#include "../include/interactive_mid_protocols/OTExtensionMalicious.hpp"

void mainR() {
	// init phase
	int baseOts = 190;
	int numOts = 700;
	int bitLength = 128;
	srand(time(NULL));
	SocketPartyData senderParty(IpAdress::from_string("127.0.0.1"), 7766);
	OTExtensionMaliciousReceiver* receiver_interface = new OTExtensionMaliciousReceiver(senderParty, numOts); // total ots

	cerr << "finished initOtReceiver." << endl;
	cerr << "Started runOtAsSender." << endl;

	maliciousot::MaskingFunction * masking_function = new maliciousot::XORMasking(bitLength);

	vector<byte> choices, response;
	//choices.Create(numOts);

	//Pre-generate the response vector for the results
	//response.Create(numOts, bitLength);

	//copy the sigma values received from java
	for (int i = 0; i<numOts; i++) {
		//choices.push_back((byte)((i / 8) * 8 + 7 - (i % 8), rand() % 2));
		choices.push_back(1);
	}

	//run the ot extension as the receiver
	OTBatchRInput * input = new OTExtensionGeneralRInput(choices, 128);
	cerr << "started receiver_interface->obliviously_receive()" << endl;
	auto output = receiver_interface->transfer(input);
	cerr << "ended receiver_interface->obliviously_receive()" << endl;

	//prepare the out array
	cerr << "response bitvector:" << endl;
	auto arr = ((OTOnByteArrayROutput*)(output.get()))->getXSigma();
	for (int i = 0; i < arr.size(); i++) {
		cout << (int) arr.at(i) << " ";
	}
	cout << endl;

	//choices.delCBitVector();
	//response.delCBitVector();

	delete masking_function;
	cerr << "ended runOtAsReceiver." << endl;
	delete receiver_interface;
}

void mainS() {
	// init phase
	int baseOts = 190;
	int numOts = 700;
	int bitLength = 128;
	srand(time(NULL));
	SocketPartyData senderParty(IpAdress::from_string("127.0.0.1"), 7766);
	OTExtensionMaliciousSender * sender_interface = new OTExtensionMaliciousSender(senderParty, numOts); // total ots
	cerr << "finished initOtSender." << endl;

	// run ot as sender phase
	vector<byte> delta, X1, X2;
	maliciousot::MaskingFunction * masking_function = new maliciousot::XORMasking(bitLength);

	//Create X1 and X2 as two arrays with "numOTs" entries of "bitlength" bit-values
	//X1.Create(numOts, bitLength);
	//X2.Create(numOts, bitLength);
	for (int i = 0; i < numOts * bitLength / 8; i++) {
		//X1.push_back(rand() % 2);
		//X2.push_back(rand() % 2);
		X1.push_back(0);
		X2.push_back(1);
	}

	OTBatchSInput * input = new OTExtensionGeneralSInput(X1, X2, numOts);
	cerr << "started receiver_interface->obliviously_send()" << endl;
	sender_interface->transfer(input); //, delta);
	cerr << "ended receiver_interface->obliviously_send()" << endl;
	//X1.delCBitVector();
	//X2.delCBitVector();
	//delta.delCBitVector();

	cerr << "ended runOtAsSender." << endl;

	delete sender_interface;
}


int mainOT(string party) {

	cout << "argv[2] = " << party << endl;
	if (party == "1") {
		mainS();
	}
	else {
		mainR();
	}
	return 1;
}