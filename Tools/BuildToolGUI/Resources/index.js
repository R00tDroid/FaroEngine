function InvokeCommand(command, project)
{
    Window.this.AppWindow.InvokeCommand(command, project);
}

function UpdateConsoleHeight()
{
    var pageHeight = document.getElementsByClassName("page")[0].offsetHeight;
    var titlebarHeight = document.getElementsByClassName("titlebar")[0].offsetHeight;
    var headerHeight = document.getElementsByClassName("headerContainer")[0].offsetHeight;
    var consoleOutput = document.getElementById("commandOutput");
    consoleOutput.style.height = pageHeight - titlebarHeight - headerHeight;
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

    window.addEventListener('resize', UpdateConsoleHeight);
    UpdateConsoleHeight();
});

Window.this.on('consoleClear', () => {
    document.getElementById("consoleText").innerHTML = "";
});

Window.this.on('consoleAppend', (event) => {
    string = event.data.replace(/(?:\r\n|\r|\n)/g, '<br>');
    consoleText = document.getElementById("consoleText");
    consoleOutput = document.getElementById("commandOutput");
    consoleText.innerHTML += string;
    consoleOutput.scrollTop = consoleText.scrollHeight;
});

Window.this.on('setProjectPath', (event) => {
    document.getElementById("projectPath").innerHTML = event.data;
});
