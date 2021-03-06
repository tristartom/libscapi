/**
* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
* 
* Copyright (c) 2016 LIBSCAPI (http://crypto.biu.ac.il/SCAPI)
* This file is part of the SCAPI project.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
* We request that any publication and/or code referring to and/or based on SCAPI contain an appropriate citation to SCAPI, including a reference to
* http://crypto.biu.ac.il/SCAPI.
* 
* Libscapi uses several open source libraries. Please see these projects for any further licensing issues.
* For more information , See https://github.com/cryptobiu/libscapi/blob/master/LICENSE.MD
*
* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
* 
*/


#pragma once
#include "CommitmentScheme.hpp"
#include "CommitmentSchemePedersen.hpp"

/**
* Concrete implementation of Equivocal commitment scheme in the committer's point of view.<p>
* This is a protocol to obtain an equivocal commitment from any commitment with a ZK-protocol of the commitment value.<p>
* The equivocality property means that a simulator can decommit to any value it needs (needed for proofs of security).<p>
*
* The pseudo code of this protocol can be found in Protocol 3.7 of pseudo codes document at {@link http://cryptobiu.github.io/scapi/SDK_Pseudocode.pdf}.<p>
*
* @author Cryptography and Computer Security Research Group Department of Computer Science Bar-Ilan University (Moriya Farbstein)
*
*/
class CmtEquivocalCommitter : public CmtCommitter, public EquivocalCmt {

	/*
	Runs the following pseudo code:
	Commit phase
	RUN any COMMIT protocol for C to commit to x
	Decommit phase, using ZK protocol of decommitment value
	SEND x to R
	Run ZK protocol as the prover, that x is the correct decommitment value
	*/

private:
	shared_ptr<CmtWithProofsCommitter> committer;

	void doConstruct(shared_ptr<CommParty> channel, shared_ptr<CmtWithProofsCommitter> committer) {
		this->committer = committer;
		this->channel = channel;
	}

public:
	/**
	* Constructor that gets committer to use in the protocol execution.
	* @param committer instance of committer that has proofs.
	*/
	CmtEquivocalCommitter(shared_ptr<CommParty> channel, shared_ptr<CmtWithProofsCommitter> committer) { doConstruct(channel, committer); }

	/**
	* Constructor that gets channel to use in the protocol execution and chooses default committer.
	* @param channel
	* @throws CheatAttemptException
	* @throws IOException
	* @throws ClassNotFoundException
	*/
	CmtEquivocalCommitter(shared_ptr<CommParty> channel, int t) { 
		doConstruct(channel, make_shared<CmtPedersenWithProofsCommitter>(channel, t));
	}

	shared_ptr<CmtCCommitmentMsg> generateCommitmentMsg(shared_ptr<CmtCommitValue> input, long id) override{
		// Delegate to the underlying committer.
		return committer->generateCommitmentMsg(input, id);
	}

	shared_ptr<CmtCDecommitmentMessage> generateDecommitmentMsg(long id) override {
		throw IllegalStateException("The Decommitment phase of this scheme is interactive. Thus, it can't generate a decommitment message. Call decommit function");
	}

	/**
	* Runs the decommit phase of the protocol.<p>
	* Pseudo code:<p>
	* "SEND x to R<p>
	*	Run ZK protocol as the prover, that x is the correct decommitment value".<p>
	*/
	void decommit(long id) override {
		//During the execution of proveCommittedValue, the x is sent to the receiver.
		committer->proveCommittedValue(id);
	} 

		/**
		* This function samples random commit value and returns it.
		* @return the sampled commit value
		*/
	shared_ptr<CmtCommitValue> sampleRandomCommitValue() override {
		//Delegate to the underlying committer.
		return committer->sampleRandomCommitValue();
	}

	/**
	* Generates CommitValue from the given byte array.
	*/
	shared_ptr<CmtCommitValue> generateCommitValue(vector<byte> x) override	{
		//Delegate to the underlying committer.
		return committer->generateCommitValue(x);
	}

	vector<shared_ptr<void>> getPreProcessValues() override {
		//Delegate to the underlying committer.
		return committer->getPreProcessValues();
	}

	/**
	* This function converts the given commit value to a byte array.
	* @param value
	* @return the generated bytes.
	*/
	vector<byte> generateBytesFromCommitValue(CmtCommitValue* value) override {
		//Delegate to the underlying committer.
		return committer->generateBytesFromCommitValue(value);
	}

};

/**
* Concrete implementation of Equivocal commitment scheme in the receiver's point of view.Pseudo code:<p>
* This is a protocol to obtain an equivocal commitment from any commitment with a ZK-protocol of the commitment value.Pseudo code:<p>
* The equivocality property means that a simulator can decommit to any value it needs (needed for proofs of security).<p>
*
* The pseudo code of this protocol can be found in Protocol 3.7 of pseudo codes document at {@link http://cryptobiu.github.io/scapi/SDK_Pseudocode.pdf}.<p>
*
*
* @author Cryptography and Computer Security Research Group Department of Computer Science Bar-Ilan University (Moriya Farbstein)
*
*/
class CmtEquivocalReceiver : public CmtReceiver, public EquivocalCmt {

	/*
	Runs the following pseudo code:
	Commit phase
	RUN any COMMIT protocol for C to commit to x
	Decommit phase, using ZK protocol of decommitment value
	Run ZK protocol as the verifier, that x is the correct decommitment value
	IF verifier-output of ZK is ACC
	OUTPUT ACC and x
	ELSE
	OUTPUT REJ

	*/

private:
	shared_ptr<CmtWithProofsReceiver> receiver;
	//shared_ptr<CommParty> channel;

	/*void doConstruct(shared_ptr<CommParty> channel, shared_ptr<CmtWithProofsReceiver> receiver) {
		this->receiver = receiver;
		this->channel = channel;
	}*/

public:
	/**
	* Constructor that gets the receiver to use in the protocol execution.
	* @param receiver
	*/
	CmtEquivocalReceiver(shared_ptr<CommParty> channel, shared_ptr<CmtWithProofsReceiver> receiver) {
		//doConstruct(channel, receiver);
		this->receiver = receiver;
	}

	/**
	* Constructor that gets channel to use in the protocol execution and chooses default receiver.
	* @param channel
	* @throws CheatAttemptException
	* @throws IOException
	* @throws ClassNotFoundException
	*/
	CmtEquivocalReceiver(shared_ptr<CommParty> channel, int t) {
		//doConstruct(channel, make_shared<CmtPedersenWithProofsReceiver>(channel, t));
		this->receiver = make_shared<CmtPedersenWithProofsReceiver>(channel, t);
	}

	/**
	* Runs the commit phase of the protocol:<P>
	* "RUN any COMMIT protocol for C to commit to x".
	*/
	shared_ptr<CmtRCommitPhaseOutput> receiveCommitment() override {
		//Delegate to the underlying receiver.
		return receiver->receiveCommitment();
	}

	/**
	* Runs the decommit phase of the protocol:<P>
	* "Run ZK protocol as the verifier, that x is the correct decommitment value<P>
	*		IF verifier-output of ZK is ACC<P>
	*          OUTPUT ACC and x<P>
	*    	ELSE<P>
	*          OUTPUT REJ".
	*/
	shared_ptr<CmtCommitValue> receiveDecommitment(long id) override{
		//During the execution of verifyCommittedValue, the x is received by the receiver.
		return receiver->verifyCommittedValue(id);
	}

	shared_ptr<CmtCommitValue> verifyDecommitment(CmtCCommitmentMsg* commitmentMsg, CmtCDecommitmentMessage* decommitmentMsg) override {
		throw IllegalStateException("The Decommitment phase of this scheme is interactive. Thus, it can't generate a decommitment message. Call decommit function");
	}

	vector<shared_ptr<void>> getPreProcessedValues() override {
		//Delegate to the underlying receiver.
		return receiver->getPreProcessedValues();
	}

	/**
	* This function converts the given commit value to a byte array.
	* @param value
	* @return the generated bytes.
	*/
	vector<byte> generateBytesFromCommitValue(CmtCommitValue* value) override {
		//Delegate to the underlying receiver.
		return receiver->generateBytesFromCommitValue(value);
	}
};
