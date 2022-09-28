const { contextBridge, ipcRenderer } = require('electron')

contextBridge.exposeInMainWorld('electronAPI',{
    runBuildTool: (command) => ipcRenderer.invoke('action:runBuildTool', command),
    onLogClear: (callback) => ipcRenderer.on('event:onLogClear', callback),
    onLogAppend: (callback) => ipcRenderer.on('event:onLogAppend', callback)
})
