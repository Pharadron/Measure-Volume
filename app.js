const express = require('express');
const body = require('body-parser');
const cookie = require('cookie-parser');
const session = require('express-session');
const mysql = require('mysql');
const connection = require('express-myconnection');
const app = express();
const path = require('path');


app.use(body.urlencoded({extended: true})); 
app.use(express.json());
app.set('view engine', 'ejs');
app.use(express.static('public'));
app.use(cookie());  
app.use(session({
    secret: '05',
    resave: false,
    saveUninitialized: true
}));


app.use(connection(mysql, {
    host: 'localhost',
    user: 'Pharadron',
    password: 'Qwe741236985',
    port: 3306,
    database: 'smartfarm38'
}, 'single'));


// const LogRoute = require('./routes/LogRoute');
// app.use('/', LogRoute);
// const Service = require('./routes/ServiceRoute');
// app.use('/',Service);
// const Person = require('./routes/PersonRoute');
// app.use('/',Person);
const IOT = require('./routes/IOTRoute');
app.use('/',IOT);

// const Deploy = require('./routes/DeployRoute');
// app.use('/',Deploy);

app.listen('8025');