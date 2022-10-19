function InvokeCommand(command, project)
{
    var platform = document.getElementById("selectPlatform");
    var config = document.getElementById("selectConfig");
    Window.this.AppWindow.InvokeCommand(command, platform.value, config.value, project);
}

function MoveWindow(dx, dy)
{
    Window.this.AppWindow.MoveWindow(dx, dy);
}

function SetWindowState(state)
{
    Window.this.AppWindow.SetWindowState(state);
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

    document.getElementById("minimizeButton").onclick = function () {
        SetWindowState("minimize");
    };

    document.getElementById("closeButton").onclick = function () {
        SetWindowState("close");
    };

    window.addEventListener('resize', UpdateConsoleHeight);
    UpdateConsoleHeight();

    document.getElementsByClassName("titlebar")[0].onmousedown = beginMouseDrag;
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

Window.this.on('lockConsole', (event) => {
    var locked = event.data === 'true';
    document.getElementById("runGenerate").disabled = locked;
    document.getElementById("runClean").disabled = locked;
    document.getElementById("runBuild").disabled = locked;
});

var dragMouseX, dragMouseY; 

function beginMouseDrag(e) {
    e = e || window.event;
    e.preventDefault();

    dragMouseX = e.clientX;
    dragMouseY = e.clientY;

    document.onmouseup = endMouseDrag;
    document.onmousemove = mouseDrag;
}

function mouseDrag(e) {
    e = e || window.event;
    e.preventDefault();

    x = e.clientX;
    y = e.clientY;

    dx = x - dragMouseX;
    dy = y - dragMouseY;

    MoveWindow(dx, dy);
}

function endMouseDrag() {
    document.onmouseup = null;
    document.onmousemove = null;
}
