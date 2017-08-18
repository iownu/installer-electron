const { app, ipcMain, BrowserWindow } = require('electron')
const { spawn } = require('child_process')
const readline = require('readline')

const path = require('path')
const url = require('url')

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let mainWindow

let consoleInstallerProcess
let ioLines
let isLoaded = false

function createWindow () {
  // Create the browser window.
  mainWindow = new BrowserWindow({width: 800, height: 600})
  mainWindow.loadURL(url.format({
    pathname: path.join(__dirname, 'loading.html'),
    protocol: 'file:',
    slashes: true
  }))

  // Open the DevTools.
//  mainWindow.webContents.openDevTools()

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
    console.log("SPAWNDE!")
    ioLines = readline.createInterface({
        input: consoleInstallerProcess.stdout,
        output: consoleInstallerProcess.stdin
    })

    ioLines.on('line', function (input) {
        if (!isLoaded) {
            mainWindow.loadURL(input)
            isLoaded = true
        } else {
            mainWindow.webContents.send("packet-from-console", JSON.parse(input))
        }
    })

    ipcMain.on("packet-to-console", function(event, arg)
    {
        consoleInstallerProcess.stdin.write(JSON.stringify(arg) + "\n")
    })
}

app.on('ready', function()
{
    createWindow()
    spawnProcess()
})

app.on('window-all-closed', function () {
  // On OS X it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== 'darwin') {
    consoleInstallerProcess.kill()
    app.quit()
  }
})

app.on('activate', function () {
  // On OS X it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (mainWindow === null) {
    createWindow()
  }
})

