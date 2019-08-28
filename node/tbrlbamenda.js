const request = require('request');
const mysql      = require('mysql');

var connection = mysql.createConnection({
	host     : 'localhost',
	user     : 'root',
	password : 'asshole',
	database : 'deku_logs'
});

connection.connect(function(err) {
  // connected! (unless `err` is set)
});

var terminalArgs = process.argv;
console.log(terminalArgs);
if(terminalArgs.length > 2) {
	for(var i = 2; i<terminalArgs.length;i++) {
		let information = terminalArgs[i];
		console.log(`[SCRIPT]: ${information}`);
		if(information == "--sms_data") {
			information = terminalArgs[i+1]
			phonenumber = terminalArgs[i+2]
			timestamp = terminalArgs[i+3]
			
			if(typeof phonenumber == "undefined" || typeof information == "undefined") {
				console.warn("[ERROR]: Information and phonenumber are not present");
				return;
			}
			let data = {
				type : 'received',
				state : 'forwarded',
				message : information,
				phonenumber : phonenumber,
				timestamp : timestamp
			}

			let query = "INSERT INTO sms_messages SET ?";
			connection.query(query, data, function(error, result) {
				if(error) {
					//::log
					console.log(`[ERROR]: ${error}`);
				}
			});
			information = JSON.parse(information);
			console.log(`[TBRL DATA]: ${information}`);
			let url = "http://127.0.0.1:8080";
			let method = "POST";
			switch(information.mhealth_data_class) {
				case "new_patient":
					url = url + "/patients/";
				break;

				case "specimen":
					url += "/patients/" + information.uuid + "/specimen";
					if(typeof information.period_2 != "undefined") {
						method = "PUT"
					}
					else {
						method = "POST";
					}
				break;

				default:
					console.warn("[SCRIPT]: tbrlbamenda.js couldn't find mhealth_data_class, returning exit");
					return;
				break;
			}
			if(typeof information.id != "undefine") delete information.id;
			if(typeof information.target_id != "undefined" ) delete information.target_id;
			if(typeof information.mhealth_data_class != "undefined") delete information.mhealth_data_class;
			console.log( `\t\t[NODE TERMINAL]: ${information}` );
			request({
				url : url,
				method : method,
				json: true,
				body: information
			}, function(error, response, body){
				if(error) {
					console.log("--->Error", error);
				} else {
					if(response.statusCode == 200) console.log("Messages forwarded successfully!");
					else console.log(response.statusCode);
				}
			});
			//res.end();
			//
			return;
		}

		else if( information == "--store_sms" ) {
			information = terminalArgs[ i+1 ];
			
			let phonenumber = terminalArgs[ i+2 ];
			let timestamp = terminalArgs[ i+3 ];
			let data = {
				type : 'received',
				message : information,
				phonenumber : phonenumber,
				timestamp : timestamp
			}

			let query = "INSERT INTO sms_messages SET ?";
			connection.query(query, data, function(error, result) {
				if(error) {
					//::log
					console.log(`[ERROR]: ${error}`);
				}
			});
			return;
		}
			
	}
	return;
}
