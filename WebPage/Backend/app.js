//Adding express library
const express = require('express');
const { param } = require('express/lib/request');
const app = express();

app.use(express.urlencoded({ extended: false }));
app.use(express.json());

//Adding sqlite library
const sqlite3 = require('sqlite3').verbose();
const DBPATH = 'DataBase/estapar.db';

//Defining hostname and port
const hostname = '10.128.64.59';
const port = 2707;

app.use(express.static("../Frontend"));
app.use(express.static("../Frontend/html"));

var plate = "YSE-5123";
var firstDate;

//Starting server
app.listen(port, hostname, () => {
	console.log(`Server running at http://${hostname}:${port}/`);
});

//Getting server status
app.get('/serverStatus')

//Get request to verify user login
app.get('/getLogin/:username/:password', (req, res) => {
	//Setting request status code
	res.statusCode = 200;
	res.setHeader('Access-Control-Allow-Origin', '*');

	//Starting database connection and setting sql code
	var db = new sqlite3.Database(DBPATH);
	var sql = `SELECT * FROM USERS WHERE USERNAME = ? AND PASSWORD = ?`;

	const { username, password } = req.params;

	//Run sql code
	db.all(sql, [username, password], (err, rows) => {
		if (err) {
			throw err;
		}

		res.send(JSON.stringify(rows));
	});

	//Closing database connection
	db.close();
});

//Get request to get all solicited valletes in database
app.get('/getAllSolicited', (req, res) => {
	//Setting request status code
	res.statusCode = 200;
	res.setHeader('Access-Control-Allow-Origin', '*');

	//Starting database connection and setting sql code
	var db = new sqlite3.Database(DBPATH);
	var sql = `SELECT * FROM SOLICITED WHERE ATIVO = "FALSE"`;

	//Run sql code
	db.all(sql, (err, rows) => {
		if (err) {
			throw err;
		}

		res.send(JSON.stringify(rows));
	});

	//Closing database connection
	db.close();
});

//Get request to get all vallets in database, except them with status "Estacionado" and "Indo estacionar"
app.get('/getAllVallets', (req, res) => {
	//Setting request status code
	res.statusCode = 200;
	res.setHeader('Access-Control-Allow-Origin', '*');

	//Starting database connection and setting sql code
	var db = new sqlite3.Database(DBPATH);
	var sql = `SELECT * FROM VALLETS WHERE STATUS != "Estacionado" AND STATUS != "Indo estacionar"`;

	//Run sql code
	db.all(sql, (err, rows) => {
		if (err) {
			throw err;
		}

		res.send(JSON.stringify(rows));
	});

	//Closing database connection
	db.close();
});

//Get request to get status from a vallet by prism
app.get('/getStatus/:prism', (req, res) => {
	//Setting request status code
	res.statusCode = 200;
	res.setHeader('Access-Control-Allow-Origin', '*');

	//Starting database connection and setting sql code
	var db = new sqlite3.Database(DBPATH);
	var sql = `SELECT ID, PRISM, STATUS FROM VALLETS WHERE PRISM = ? AND STATUS != "Carro devolvido"`;

	//Getting prism number from request parameter
	const { prism } = req.params;

	var param = [];
	param.push(prism);

	//Run sql code
	db.all(sql, param, (err, rows) => {
		if (err) {
			throw err;
		}

		res.send(JSON.stringify(rows));
	});

	//Closing database connection
	db.close();
});

//Get request to select all fineshed vallets
app.get('/getStatistics', (req, res) => {
	//Setting request status code
	res.statusCode = 200;
	res.setHeader('Access-Control-Allow-Origin', '*');

	//Starting database connection and setting sql code
	var db = new sqlite3.Database(DBPATH);
	var sql = `SELECT
					VALLETS.*,
					VP1.NAME AS NAME1,
					VP2.NAME AS NAME2
				FROM VALLETS
					INNER JOIN VALLET_PARKING AS VP1 ON VP1.ID = VALLETS.VALLET_PARKING1_ID
					INNER JOIN VALLET_PARKING AS VP2 ON VP2.ID = VALLETS.VALLET_PARKING2_ID
				WHERE VALLETS.STATUS = "Carro devolvido"`;

	//Getting prism number from request parameter
	const { prism } = req.params;

	var param = [];
	param.push(prism);

	//Run sql code
	db.all(sql, param, (err, rows) => {
		if (err) {
			throw err;
		}

		res.send(JSON.stringify(rows));
	});

	//Closing database connection
	db.close();
});

function padTo2Digits(num) {
	return num.toString().padStart(2, '0');
}

//Function to convert date variable type to string "yyyy-mm-dd HH:MM:SS"
function formatDate(date) {
	return (
	  [
		date.getFullYear(),
		padTo2Digits(date.getMonth() + 1),
		padTo2Digits(date.getDate()),
	  ].join('-') +
	  ' ' +
	  [
		padTo2Digits(date.getHours()),
		padTo2Digits(date.getMinutes()),
		padTo2Digits(date.getSeconds()),
	  ].join(':')
	);
  }

//Post request to add solicited vallets to database
app.post('/addValletToTotem', async (req, res) => {
	plate = req.body.Plate;

	//Setting request status code
	res.statusCode = 200;
	res.setHeader('Access-Control-Allow-Origin', '*');

	//Starting database connection and setting sql code
	var db = new sqlite3.Database(DBPATH);
	var sql = `INSERT INTO SOLICITED (PLATE, ATIVO) VALUES(?, "TRUE")`;

	var param = [plate];

	//Run sql code
	db.all(sql, param, (err, rows) => {
		if (err){
			throw err;
		}

		res.json(rows);
	})

	//Closing database connection
	db.close();
});

//Post request to inset a vallet in database
app.post('/postVallet', async (req, res) => {
	//Setting request status code
	res.statusCode = 200;
	res.setHeader('Access-Control-Allow-Origin', '*');

	//Starting database connection and setting sql code
	var db = new sqlite3.Database(DBPATH);
	var sql = `INSERT INTO VALLETS (OPERATION, PRISM, PLATE, STATUS, VALLET_PARKING1_ID, TIME1_VP1) VALUES(?, ?, ?, "Indo estacionar", 1, ?)`;

	//Call function to transform date to string
	var data = formatDate(new Date());

	var param = [req.body.Operation, req.body.Prism, plate, data];

	console.log(req.body);

	//Run sql code
	db.all(sql, param, (err, rows) => {
		if (err){
			throw err;
		}

		res.json(rows);
	})

	//Closing database connection
	db.close();
});

//Function to get time to find car
function timeToFind(date1, date2){
	var firstMin = date1.getHours() * 60;
    var firstMin = firstMin + date1.getMinutes();

    var lastMin = date2.getHours() * 60;
    var lastMin = lastMin + date2.getMinutes();

    var absoluteMinutes = lastMin - firstMin;

	return absoluteMinutes;
}

//Function to choose sql code accord to actual status of vallet
function SqlCode(status, idValledParking, idVallet, date){
	var strSql = ``;
	var param = [];

	switch (status)
	{
		case "Indo estacionar":
			strSql = `UPDATE VALLETS SET STATUS = "Estacionado", TIME2_VP1 = ? WHERE ID = ?`;
			param.push(date, idVallet);

			break;
		
		case "Estacionado":
			strSql = `UPDATE VALLETS SET STATUS = "Retornando carro ao cliente", VALLET_PARKING2_ID = ?, TIME1_VP2 = ?, FIND_TIME = ? WHERE ID = ?`;
			param.push(2, date, timeToFind(firstDate, new Date()), idVallet);

			break;

		case "Retornando carro ao cliente":
			strSql = `UPDATE VALLETS SET STATUS = "Carro devolvido", TIME2_VP2 = ? WHERE ID = ?`;
			param.push(date, idVallet);

			break;
	}

	return [strSql, param];
}

//Put request to update solcited valet status
app.post('/updateSolicitedVallets', async (req, res) => {
	//Setting request status code
	res.statusCode = 200;
	res.setHeader('Access-Control-Allow-Origin', '*');

	firstDate = new Date();

	//Starting database connection and setting sql code
	var db = new sqlite3.Database(DBPATH);
	var sql = `UPDATE SOLICITED SET ATIVO = ? WHERE PLATE = ?`;

	var param = [];
	param.push(req.body.Status);
	param.push(req.body.Plate);

	//Run sql code
	db.all(sql, param, (err, rows) => {
		if (err) {
			throw err;
		}

		res.json(rows);
	});

	//Closing database connection
	db.close();
});

//Put request to update vallet status
app.put('/updateValletStatus', async (req, res) => {
	//Setting request status code
	res.statusCode = 200;
	res.setHeader('Access-Control-Allow-Origin', '*');

	var actualStatus = {
		"Id": req.body.IdVallet,
		"Status": req.body.Status
	};

	date = formatDate(new Date())

	//Starting database connection and setting sql code
	var db = new sqlite3.Database(DBPATH);
	var [sql, param] = SqlCode(actualStatus.Status, req.body.IdValletParking, actualStatus.Id, date);

	console.log(req.body);

	//Run sql code
	db.all(sql, param, (err, rows) => {
		if (err) {
			throw err;
		}

		res.json(rows);
	});

	//Closing database connection
	db.close();
});

//Put request to update vallet time to finish
app.put('/updateValletTime', async (req, res) => {
	//Setting request status code
	res.statusCode = 200;
	res.setHeader('Access-Control-Allow-Origin', '*');

	//Starting database connection and setting sql code
	var db = new sqlite3.Database(DBPATH);
	var sql = `UPDATE VALLETS SET TIME = ? WHERE ID = ?`;

	console.log(req.body);

	var param = []
	param.push(req.body.Time, req.body.IdVallet);

	//Run sql code
	db.all(sql, param, (err, rows) => {
		if (err) {
			throw err;
		}

		res.json(rows);
	});

	//Closing database connection
	db.close();
});