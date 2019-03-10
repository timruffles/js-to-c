'use strict'

const express = require('express')

const app = require('app')
const util = require('util')
const fs = util.promisify(require('fs'))
const childProcess = util.promisify(require('child_process'))

class ErrorWithStatus extends Error {
    constructor(message, status) {
        super(message)
        this.status = status
    }
}

app.post('/compile', async (req, res, next) => {

    let target
    try {
        const dir = await fs.mkdtempAsync('compile-')
        target = `${dir}/program.js`
        await fs.writeFileAsync(target, {encoding: 'utf8'})
    } catch(e) {
        return next(new ErrorWithStatus("internal error", 500))
    }

    try {
        const { stdout, stderr } = await childProcess.spawnAsync()

    } catch(e) {

    }



})
