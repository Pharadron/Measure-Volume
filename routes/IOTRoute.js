const express = require('express');
const router = express.Router();
const IOTController = require('../controllers/IOTController');
// const validator = require('../controllers/validatorIOT');


router.get('/iot38', IOTController.Dist);

module.exports = router;