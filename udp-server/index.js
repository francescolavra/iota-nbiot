#!/usr/bin/env node
'use strict';

const IOTA = require('iota.lib.js');

const data = {
	accountData: undefined,
	currentNodeInfo: undefined,
	depth: 9,
	minWeightMagnitude: 3,
	seed: 'TEYWQFLIRHJHIMWLJUNLJSSXRIK9SFZSCMYJJDFOVUEMDLTAPAMMSUVKWRQUUFZLQDTDMKLTPJYEHDKCM',
	recipientAddress: 'SMPGSEHMHJTCKYXRG9CLWFWQEKURODZOAOVBTWMY9RPZDKSUTKKTTTNPROXRMPJXKUYJGNXXUGIRPUOLO'
};

const iotajs = new IOTA({
	host: 'http://iota.bitfinex.com',
	port: 80
});

let refreshAccountDataTimer;
const refreshAccountData = () => {
	if (refreshAccountDataTimer) {
		clearTimeout(refreshAccountDataTimer);
	}

	iotajs.api.getAccountData(data.seed, (err, accountData) => {
		if (err) {
			// on fail, retry fast
			refreshAccountDataTimer = setTimeout(refreshAccountData, 10 * 1000);
			return;
		}

		if (!data.accountData) {
			console.log('Account data retrieved.');
		}
		data.accountData = accountData;

		// on success, refresh slowly.
		refreshAccountDataTimer = setTimeout(refreshAccountData, 2 * 60 * 1000);
	});
};

const refreshServerInfo = () => {
	iotajs.api.getNodeInfo((err, nodeInfo) => {
		if (err) {
			data.currentNodeInfo = undefined;
		} else {
			data.currentNodeInfo = nodeInfo;
		}
	});
};

function udpServerStart(cb, req) {
	const dgram = require('dgram');
	const server = dgram.createSocket('udp4');

	server.on('error', (err) => {
		server.close();
		cb('server error:\n${err.stack}');
	});
	server.on('message', (msg, rinfo) => {
		var resp = req(msg, rinfo.address, rinfo.port);
		server.send(JSON.stringify(resp), rinfo.port, rinfo.address);
	});
	server.bind(14265);
	cb(null);
}

function transferRequest(msg) {
	var jsonMsg;
	var resp = {};

	try {
		jsonMsg = JSON.parse(msg);
	} catch(err) {
		resp.result = 'error';
		resp.error = err.message;
		return resp;
	}
	if (jsonMsg.command !== 'transfer') {
		resp.result = 'error';
		resp.error = 'invalid command';
	}
	else {
		if ((typeof jsonMsg.iota != 'number') || !Number.isInteger(jsonMsg.iota)
				|| (jsonMsg.iota <= 0)) {
			resp.result = 'error';
			resp.error = 'invalid IOTA amount';
		}
		else {
			const transfers = [{
				address: data.recipientAddress,
				value: jsonMsg.iota,
				message: '',
				tag: ''
			}];

			iotajs.api.sendTransfer(data.seed, data.depth,
					data.minWeightMagnitude, transfers, err => {
						if (err) {
							console.log('Error when sending transfer:',
									err.message);
						}
						else {
							console.log('Transfer complete');
						}
					});
			resp.result = 'ok';
		}
	}
	return resp;	
}

console.log('Connecting to node', iotajs.host);
refreshAccountData();

data.recipientAddress = iotajs.utils.addChecksum(data.recipientAddress);

udpServerStart((err) => {
	if (err) {
		console.log('Cannot start UDP server:', err);
		process.exit(1);
	}
}, transferRequest);

setTimeout(refreshServerInfo, 100);
setInterval(refreshServerInfo, 15 * 1000);
