const express = require('express');
const app = express();

app.use(express.urlencoded({ extended: false }));
app.use(express.json());

const sqlite3 = require('sqlite3').verbose();
const DBPATH = 'DataBase/estapar.db';

// const hostname = '127.0.0.1';
const hostname = '10.128.64.231';
// const hostname = '192.168.106.1';
const port = 2707;

app.use(express.static("../Frontend"));
app.use(express.static("../Frontend/html"));

app.listen(port, hostname, () => {
	console.log(`Server running at http://${hostname}:${port}/`);
});

app.get('/serverStatus')

app.get('/getAllVallets', (req, res) => {
	res.statusCode = 200;
	res.setHeader('Access-Control-Allow-Origin', '*');

	var db = new sqlite3.Database(DBPATH);
	var sql = `SELECT * FROM VALLETS WHERE STATUS != "Estacionado" AND STATUS != "Indo estacionar"`;

	db.all(sql, (err, rows) => {
		if (err) {
			throw err;
		}

		res.send(JSON.stringify(rows));
	});

	db.close();
});

app.get('/getStatus/:prism', (req, res) => {
	res.statusCode = 200;
	res.setHeader('Access-Control-Allow-Origin', '*');

	var db = new sqlite3.Database(DBPATH);
	var sql = `SELECT ID, PRISM, STATUS FROM VALLETS WHERE PRISM = ? AND STATUS != "Carro devolvido"`;

	const { prism } = req.params;

	var param = [];
	param.push(prism);

	db.all(sql, param, (err, rows) => {
		if (err) {
			throw err;
		}

		res.send(JSON.stringify(rows));
	});

	db.close();
});

function padTo2Digits(num) {
	return num.toString().padStart(2, '0');
}

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

app.post('/postVallet', async (req, res) => {
	res.statusCode = 200;
	res.setHeader('Access-Control-Allow-Origin', '*');

	var db = new sqlite3.Database(DBPATH);
	var sql = `INSERT INTO VALLETS (PRISM, PLATE, STATUS, VALLET_PARKING1_ID, TIME1_VP1) VALUES(?, "FYZ-3642", "Indo estacionar", 1, ?)`;

	var data = formatDate(new Date());

	var param = [req.body.Prism, data];

	console.log(req.body);

	db.all(sql, param, (err, rows) => {
		if (err){
			throw err;
		}

		res.json(rows);
	})

	db.close();
});

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
			strSql = `UPDATE VALLETS SET STATUS = "Retornando carro ao cliente", VALLET_PARKING2_ID = ?, TIME1_VP2 = ? WHERE ID = ?`;
			param.push(idValledParking, date, idVallet);

			break;

		case "Retornando carro ao cliente":
			strSql = `UPDATE VALLETS SET STATUS = "Carro devolvido", TIME2_VP2 = ? WHERE ID = ?`;
			param.push(date, idVallet);

			break;
	}

	return [strSql, param];
}

app.put('/updateValletStatus', async (req, res) => {
	res.statusCode = 200;
	res.setHeader('Access-Control-Allow-Origin', '*');

	var actualStatus = {
		"Id": req.body.IdVallet,
		"Status": req.body.Status
	};

	date = formatDate(new Date())

	var db = new sqlite3.Database(DBPATH);
	var [sql, param] = SqlCode(actualStatus.Status, req.body.IdValletParking, actualStatus.Id, date);

	console.log(req.body);

	db.all(sql, param, (err, rows) => {
		if (err) {
			throw err;
		}

		res.json(rows);
	});
	db.close();
});

app.put('/updateValletTime', async (req, res) => {
	res.statusCode = 200;
	res.setHeader('Access-Control-Allow-Origin', '*');

	date = formatDate(new Date())

	var db = new sqlite3.Database(DBPATH);
	var sql = `UPDATE VALLETS SET TIME = ? WHERE ID = ?`;

	console.log(req.body);

	var param = []
	param.push(req.body.Time, req.body.IdVallet);

	db.all(sql, param, (err, rows) => {
		if (err) {
			throw err;
		}

		res.json(rows);
	});
	db.close();
});