var express = require('express')
	,app = express()
	,server = require('http').createServer(app)
	,io = require('socket.io')(server)
	,cors = require('cors')
	,cons = require('consolidate')
	,status = false
	,port = process.env.PORT || 8080;

var corsOptions = {
	origin: [
		'http://springbroad.dev',
		'https://springbroad-ff92f.firebaseapp.com',
		'chrome-extension://olmpdogecinnhjjmaijikaiepaopekgh'
	],
	credentials: true
}

//Server Cors to entire site
app.use(cors(corsOptions));


app.get('/', function (req, res) {
  
	res.status(200).json({ isOpen: status });
	
});

app.get('/getStatus', function (req, res) {

  	var text = "The restroom is open";

  	if(status == "false"){
  		text = "The restroom is closed";
  	}

	var json = {
		text: text
	}
	
	res.status(200).json(json);
});

app.get('/setStatus', function (req, res) {

	var response = JSON.parse(JSON.stringify(req.query));

	status = response.isopen;
	io.emit('outhouse', { isOpen: status });
	res.json({ isOpen: status });
	
});

io.on('connection', function(socket){
	console.log("You have a connection");
});

server.listen(port, function () {
  console.log('Outhouse is listening on port %s!', port);
});