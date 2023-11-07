const express = require('express');
const mysql = require('mysql');
const bodyParser = require('body-parser');

const app = express();
app.set('view engine', 'ejs');
app.use(bodyParser.urlencoded({ extended: true }));

// MySQL configuration
const connection = mysql.createConnection({
  host: 'localhost',
  user: 'user',
  password: 'user1.password',
  database: 'fmv_food_db',
});

// Connect to MySQL
connection.connect((err) => {
  if (err) {
    console.error('Error connecting to MySQL: ', err);
    return;
  }
  console.log('Connected to MySQL database');
});

// Route for the home page
app.get('/', (req, res) => {
  res.render('index');
});

// Route to handle form submission
app.post('/search', (req, res) => {
  const number = req.body.number;

  // Search for a list in the database
  const query = 'SELECT * FROM lists WHERE number = ?';
  connection.query(query, [number], (err, results) => {
    if (err) {
      console.error('Error executing MySQL query: ', err);
      return;
    }
    res.render('result', { lists: results });
  });
});

// Start the server
app.listen(3000, () => {
  console.log('Server started on port 3000');
});





// const http = require('http');

// const hostname = 'localhost';
// const port = 3000;

// const server = http.createServer((req, res) => {
//   res.statusCode = 200;
//   res.setHeader('Content-Type', 'text/plain');
//   res.end('Hello World!');
// });

// server.listen(port, hostname, () => {
//   console.log(`Server running at http://${hostname}:${port}/`);
// });