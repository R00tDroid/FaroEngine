function InvokeCommand(command)
{
    Window.this.AppWindow.InvokeCommand(command);
}

window.addEventListener('DOMContentLoaded', () => {
    document.getElementById("runGenerate").onclick = function(){
        InvokeCommand("-generate");
    };

    document.getElementById("runClean").onclick = function(){
        InvokeCommand("-clean"); 
    };

    document.getElementById("runBuild").onclick = function(){
        InvokeCommand("-build"); 
    };
});

Window.this.on('consoleClear', () => {
    document.getElementById("commandOutput").innerHTML = "";
});

Window.this.on('consoleAppend', (event) => {
    string = event.data.replace(/(?:\r\n|\r|\n)/g, '<br>');
    document.getElementById("commandOutput").innerHTML += string;
});

Window.this.on('setProjectPath', (event) => {
    document.getElementById("projectPath").innerHTML = event.data;
});
