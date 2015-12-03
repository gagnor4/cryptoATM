/*
  atm.cpp - ATM
*/
#include "atm.h"

#define CARD_LOC "cards/"
#define PINSTR "PIN:"
#define NSTR "N:"
#define ESTR "E:"
#define DSTR "D:"

User::User(string s, Integer n, Integer e, Integer d) {
  name = s;
  pr.Initialize(n, d, e);
  pub.Initialize(n, e);
}

User::User(string s, RSA::PrivateKey k, RSA::PublicKey p) {
  name = s;
  pr = k;
  pub = p;
}

ATM::ATM(int p) {
  buffer = new char[MSG_LEN];
  message = new char[MSG_LEN];

  AutoSeededRandomPool rnd;  
  LoadPublicKey("atm/bank-public.key", bankpub);
  if (!bankpub.Validate(rnd, 3)) {
    cout << "Could not validate key" << endl;
  }

  bool authenticated = false;
  int i = 0;
  while (!authenticated && i < 10) {
    socket = new Socket(p);

    atmkey.GenerateRandomWithKeySize(rnd, KEY_LEN);
    atmpub = RSA::PublicKey(atmkey);

    if (!atmkey.Validate(rnd, 3)) {
      cout << "Could not validate key" << endl;
    }

    user = 0;
    session = 0;

    authenticated = authenticate_atm();
    if (!authenticated) {
      delete socket;
      usleep(100);
      ++i;
    }
  }
}

ATM::~ATM() {
  delete socket;

  delete[] buffer;
  delete[] message;
}

bool ATM::login(string name, int pin) {
  ifstream card(string(CARD_LOC + name + ".card").c_str());
  if (!card) {
    cout << "Could not find card file for user" << endl;
    return false;
  }
  string tstr;
  int tpin;
  Integer n, e, d;
  
  string line;
  stringstream* stream;

  // Read PIN number
  getline(card,line);
  stream = new stringstream(line);
  if (!read_string(stream, tstr)) return false;
  if (tstr != PINSTR) return false;
  if (!read_positive_int(stream, tpin)) return false;
  if (pin != tpin) return false;
  delete stream;

  return login(name);
}

bool ATM::login(string name) {
  bool valid = false;
  RSA::PrivateKey k;
  RSA::PublicKey p, p2;
  AutoSeededRandomPool rnd;
  while (!valid) {
    k.GenerateRandomWithKeySize(rnd, KEY_LEN);
    p = RSA::PublicKey(k);
    
    save_key_to_buffer(p, buffer);
    RSA_encrypt(buffer, message, atmpub);
    RSA_decrypt(message, buffer, atmkey);
    load_key_from_buffer(p2, buffer);

    valid = (p.GetModulus() == p2.GetModulus());
  }
  user = new User(name, k, p);
  valid = false;
  int i = 0;
  while (!authenticate_user() && i < 10) {
    ++i;
  }
  if (i < 10) {
    return true;
  }
  else {
    cout << "Could not validate user" << endl;
    return false;
  }
}

bool ATM::loggedin() {
  return (user != 0) && (session != 0);
}

string ATM::get_username() {
  if (!user) {
    return "";
  }
  return user->get_name();
}

void ATM::logout() {
  delete user;
  user = 0;
}

int ATM::balance() {
  return send(user->get_name(), 0);
}

int ATM::withdraw(int amount) {
  return send(user->get_name(), 0-amount);
}

int ATM::transfer(string name, int amount) {
  int response = send(user->get_name(), 0-amount);
  if (response >= 0) {
    sleep(1);
    return send(name, amount);
  }
  else {
    return response;
  }
}

bool ATM::authenticate_atm() {
  save_key_to_buffer(atmpub, buffer);
  RSA_encrypt(buffer, message, bankpub);
  send_message(message);
  recv_message(message);

  if (message[0] == '\0') {
    //cout << "Bad public key, retrying" << endl;
    return false;
  }
  
  RSA_decrypt(message, buffer, atmkey);

  session = new Session();
  session->time = get_current_time();

  char* testvalid = new char[16];
  
  int offset = 0;
  memcpy(session->key, &buffer[offset], AES::DEFAULT_KEYLENGTH);
  offset += AES::DEFAULT_KEYLENGTH;
  memcpy(session->iv, &buffer[offset], AES::BLOCKSIZE);
  offset += AES::BLOCKSIZE;
  memcpy(testvalid, &buffer[offset], 16);
  offset += 16;

  if (strncmp(testvalid, valid_message, 16) != 0) {
    //cout << "Invalid message" << endl;
    return false;
  }
  
  return true;
}

bool ATM::authenticate_user() {
  int header = create_header(true, buffer);
  pad(header, buffer);

  AES_encrypt(buffer, MSG_LEN, session->key, session->iv);
  
  send_message(buffer);
  recv_message(message);
  if (message[0] == '\0') {
    //cout << "Bad message" << endl;
    return false;
  }

  AES_decrypt(message, MSG_LEN, session->key, session->iv);

  // Message with just timestamp

  // Send public key

  save_key_to_buffer(user->get_public_key(), buffer);

  RSA_encrypt(buffer, message, bankpub);
  AES_encrypt(message, MSG_LEN, session->key, session->iv);

  send_message(message);
  recv_message(message);
  if (message[0] == '\0') {
    //cout << "Bad public key" << endl;
    return false;
  }
  
  AES_decrypt(message, MSG_LEN, session->key, session->iv);

  RSA::PrivateKey ukey = user->get_private_key();
  RSA_decrypt(message, buffer, ukey);

  memset(user->key, 0, AES::DEFAULT_KEYLENGTH);
  memset(user->iv, 0, AES::BLOCKSIZE);

  char* testvalid = new char[16];
  
  int offset = 0;
  memcpy(user->key, &buffer[offset], AES::DEFAULT_KEYLENGTH);
  offset += AES::DEFAULT_KEYLENGTH;
  memcpy(user->iv, &buffer[offset], AES::BLOCKSIZE);
  offset += AES::BLOCKSIZE;
  memcpy(testvalid, &buffer[offset], 16);
  offset += 16;

  if (strncmp(testvalid, valid_message, 16) != 0) {
    //cout << "Invalid message" << endl;
    return false;
  }
  return true;
}

int ATM::send(string name, int amount) {
  create_message(name, amount, buffer);
  send_message(buffer);
  recv_message(message);
  AES_decrypt(message, MSG_LEN, user->key, user->iv);
  AES_decrypt(message, MSG_LEN, session->key, session->iv);
  return read_message(message);
}

int ATM::create_message(string name, int amount, char* buf) {
  int header = create_header(false, buf);
  int body = create_body(name, amount, header, buf);
  pad(body, buf);

  AES_encrypt(&buf[header], MSG_LEN - header, user->key, user->iv);
  
  AES_encrypt(buf, MSG_LEN, session->key, session->iv);

  return MSG_LEN;
}

int ATM::create_header(bool auth, char* buf) {
  memset(buf, 0, MSG_LEN);
  string  name = user->get_name();
  unsigned int strlen = name.length();
  int offset = 0;
  memcpy(&buf[offset], &auth, sizeof(auth));
  offset += sizeof(auth);
  memcpy(&buf[offset], &strlen, sizeof(strlen));
  offset += sizeof(strlen);
  memcpy(&buf[offset], name.data(), strlen);
  offset += strlen;

  return offset;
}

int ATM::create_body(string name, int amount, int offset, char* buf) {
  long long time = get_current_time();
  memcpy(&buf[offset], &time, sizeof(time));
  offset += sizeof(time);
  memcpy(&buf[offset], &amount, sizeof(amount));
  offset += sizeof(amount);
  unsigned int strlen = name.length();
  memcpy(&buf[offset], &strlen, sizeof(strlen));
  offset += sizeof(strlen);
  memcpy(&buf[offset], name.data(), strlen);
  offset += strlen;
  
  return offset;
}

int ATM::read_message(char* buf) {
  int offset = 0;
  long long time;
  int response;
  memcpy(&time, &buf[offset], sizeof(time));
  offset += sizeof(time);
  memcpy(&response, &buf[offset], sizeof(response));
  offset += sizeof(response);

  if (!valid_timestamp(time)) {
    //cout << "Invalid time" << endl;
    return -1;
  }
  return response;
}

bool ATM::valid_timestamp(long long t) {
  if (session) {
    if (t > session->time) {
      session->time = t;
      return true;
    }
  }
  return false;
}

bool ATM::send_message(char* buf) {
  if (socket->writen(buf, MSG_LEN) != MSG_LEN) {
    cout << "Failed write" << endl;
    return false;
  }
  return true;
}

bool ATM::recv_message(char* buf) {
  memset(buf, 0, MSG_LEN);
  if (socket->readn(buf, MSG_LEN) != MSG_LEN) {
    cout << "Failed read" << endl;
    return false;
  }
  return true;
}


