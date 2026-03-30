const express = require('express');
const readingsRouter = require('./routes/readings');
const cors = require('cors');

const app = express();

app.use(cors());
app.use(express.json());
app.use(readingsRouter);

app.get('/health', (_, res) => {
  res.json({ status: 'ok' });
});

module.exports = app;
