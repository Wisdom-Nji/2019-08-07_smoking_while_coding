const request = require('request');
var terminalArgs = process.argv;
console.log(terminalArgs);
if(terminalArgs.length > 2) {
	for(var i = 2; i<terminalArgs.length;i++) {
		let information = terminalArgs[i];
		console.log(`[SCRIPT]: ${information}`);
		if(information == "--sms_data") {
			information = terminalArgs[i+1]
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
		}
	}
	return;
}
