const { contextBridge, ipcRenderer } = require('electron')

contextBridge.exposeInMainWorld('electronAPI',{
    runBuildTool: () => ipcRenderer.invoke('action:runBuildTool'),
    onLogClear: (callback) => ipcRenderer.on('event:onLogClear', callback),
    onLogAppend: (callback) => ipcRenderer.on('event:onLogAppend', callback)
})
