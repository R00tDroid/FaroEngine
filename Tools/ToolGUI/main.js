const { app, BrowserWindow, ipcMain } = require('electron');
const path = require('path');
var child_process = require('child_process');
var win;

const createWindow = () => {
    win = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js'),
        },
    });
    
    win.loadFile('index.html');
};

app.whenReady().then(() => {
    
    ipcMain.handle('action:runBuildTool', runBuildTool)
    
    createWindow();
    
    app.on('activate', () => {
        if (BrowserWindow.getAllWindows().length === 0) {
            createWindow();
        }
    });
});

function logClear()
{
    win.webContents.send('event:onLogClear')
}
function logAppend(string)
{
    string = string.replace(/(?:\r\n|\r|\n)/g, '<br>');
    win.webContents.send('event:onLogAppend', string)
}

async function runBuildTool()
{
    logClear();
    
    try
    {
        var executablePath = "D:\\Projects\\FaroEngine\\Bin\\FaroBuildTool.exe";
        var parameters = ["-generate", "-build", "-project", "D:\\Projects\\FaroEngine\\Source\\FaroEngine.faroproject.json", "-platform", "windows" ,"x64", "-debug"];
        
        logAppend("Command: " + executablePath + "\n");
        logAppend("Parameters: " + parameters + "\n");
        
        const process = child_process.spawn(executablePath, parameters);
        
        process.stdout.on('data', stdout => {
            logAppend(stdout.toString());
        });
        process.stderr.on('data', stderr => {
            logAppend(stderr.toString());
        });
        process.on('error', err => {
            logAppend(err);
        })
        process.on('close', code => {
            logAppend("Exit: " + code.toString());
        });
    }
    catch(error)
    {
        logAppend(error.toString());
    }
}

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit();
    }
});
