const { app, ipcMain, BrowserWindow, dialog } = require('electron') // We do not use dialog, but https://stackoverflow.com/questions/37808294/cannot-find-module-dialog-electron-fatal-error
const { spawn } = require('child_process')
const readline = require('readline')

const path = require('path')
const url = require('url')

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let mainWindow

let consoleInstallerProcess
let ioLines

function createWindow () {
  // Create the browser window.
  mainWindow = new BrowserWindow({width: 600, height: 930})
  mainWindow.loadURL(url.format({
    pathname: path.join(__dirname, 'index.html'),
    protocol: 'file:',
    slashes: true
  }))

  // Open the DevTools.
  //mainWindow.webContents.openDevTools()

  // Emitted when the window is closed.
  mainWindow.on('closed', function () {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null
  })
}

function spawnProcess() {
    consoleInstallerProcess = spawn(path.join(__dirname, "consoleInstaller"))   
    ioLines = readline.createInterface({
        input: consoleInstallerProcess.stdout,
        output: consoleInstallerProcess.stdin
    })

    ioLines.on('line', function (input) {
		if (mainWindow) {
            mainWindow.webContents.send("packet-from-console", JSON.parse(input))
		}
    })

    ipcMain.on("packet-to-console", function(event, arg) {
        consoleInstallerProcess.stdin.write(JSON.stringify(arg) + "\n")
    })
}

app.on('ready', function() {
    createWindow()
})

ipcMain.on('ready', function(event) {
    spawnProcess()
})

app.on('window-all-closed', function () {
    consoleInstallerProcess.kill()
    app.quit()
})
