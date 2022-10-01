function InvokeCommand(command, project)
{
    Window.this.AppWindow.InvokeCommand(command, project);
}

window.addEventListener('DOMContentLoaded', () => {
    document.getElementById("runGenerate").onclick = function(){
        InvokeCommand("-generate", document.getElementById("projectPath").innerHTML);
    };

    document.getElementById("runClean").onclick = function(){
        InvokeCommand("-clean", document.getElementById("projectPath").innerHTML);
    };

    document.getElementById("runBuild").onclick = function(){
        InvokeCommand("-build", document.getElementById("projectPath").innerHTML);
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
