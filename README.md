# cryptoATM

Authors: David Hedin, Daniel Fitzgerald, Ryan Gagnon

## Protocol Description

Preliminary step: Bank public key is a shared secret known by all ATMs. All ATMs have access to users .card files, which store a user's PIN.

#### Key:
* A - ATM
* B - Bank
* U - User
* Xpub - Public key for entity X
* Xk - AES key for entity X
* Xiv - AES nonce for entity X
* E(Xpub)(D) - D is RSA encrypted using X's public key
* E(Xk)(D) - D is AES encrypted using X's AES key and nonce

### User - ATM authentication
1. User enters name.
2. User enters pin number.
3. ATM checks if password is right.
* If Password is right, connection is established.
* If Password is wrong, authentication fails.

### Authenticate ATM with Bank:
1. ATM sends their public key to bank by encrypting the ATM’s public key with the Bank public key.
[A->B: E(Bpub)(Apub)]
2. Bank decrypts ATM’s public key with bank private key.
3. Bank generates ATM’s AES session key and a nonce.
4. Bank encrypts AES key and nonce with ATM’s public key and send it to the ATM. 
[B->A: E(Apub)(Ak, Aiv)]

### Authenticate User with Bank:
1. ATM generates an authentication message asking for user authentication.
2. ATM encrypts the message with ATM’s AES session key and sends it to the bank.
3. The bank acknowledges the message and prepares to receive the user's public key.
3. ATM encrypts user’s public key with the Bank's public key and ATM’s AES key and sends it to the bank.
[A->B: E(Ak)(E(Bpub)(Upub))]
4. Bank generates user’s AES session key and nonce.
5. Bank encrypts the user’s AES session key and nonce with the user’s public key
6. Encrypted user AES session key and nonce is encrypted again with the ATM’s AES session key and sent to the ATM
[B->A: E(Ak)(E(Upub)(Uk, Uiv))]

### Sending data
1. ATM generates data based on user input, containing:
* Timestamp
* Dollar amount
* User to perform transaction on
2. ATM encrypts the data with the Users’s AES session key.
[D = E(Uk)(time, dollar, user)]
3. ATM generates a header for the data, containing:
* Name of sender
* Type of message (data/auth)
4. ATM encrypts the entire message with the ATM's AES session key.
[M = E(Ak)(sender, type, D)]
5. ATM sends the message to the Bank
[A->B: M]
6. Bank reads message and generates a response, containing:
* Timestamp
* Response
7. Bank encrypts this using both AES session keys and sends it to the ATM
[B->A: E(Ak)(E(Uk)(time, response))]

## Benefits of Protocol
* Man in the Middle attacks are prevented because the ATM's public key is only share after it has been encrypted using the Bank's pre-share public key, thus ensuring only the Bank will know it. After both have a public key, and AES key is shared so that messages may be encrypted quickly and less information about the RSA keys is available.
* After the ATM and Bank authenticate with each other, each User is authenticated in the same method, but this time using the ATM's AES session on top of the Bank's public key. Encrypting the User's public key with the Bank's public key prevents other users from being able to read the User's key.
* Each User has a unique AES session key, thus are unable to read other user's messages.
* Each message is encrypted twice, so even if the ATM's AES session key is compromised, the attacker would still only be able to access general information, and not the User important data.
* Both the ATM and Bank store last received timestamp, and if it receives a timestamp that is newer, it replaces the last received with the new timestamp. If it receives a timestamp that is older than the last received, it discards the message, preventing basic replay attacks.

## Install
1. Run the install script

## Running
* The bank must be started and allowed to set up before starting the proxy, which must be set up before starting the ATM.
* No program can bind to an already used port, if there is an error, switch to a different port
* The programs should be run from the cryptoATM directory, otherwise they may not find files correctly. Run like this:

`
bin/bank <port>
`

All executables are in the bin directory
