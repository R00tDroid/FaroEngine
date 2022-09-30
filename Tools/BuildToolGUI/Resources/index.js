window.addEventListener('DOMContentLoaded', () => {
    const thisWindow = Window.this;

    document.getElementById("runGenerate").onclick = function(){
        thisWindow.frame.InvokeCommand("-generate");
    };

    document.getElementById("runClean").onclick = function(){
        thisWindow.frame.InvokeCommand("-clean"); 
    };

    document.getElementById("runBuild").onclick = function(){
        thisWindow.frame.InvokeCommand("-build"); 
    };
});

Window.this.on('consoleClear', () => {
    document.getElementById("commandOutput").innerHTML = "";
});

Window.this.on('consoleAppend', (p1) => {
    string = p1.data.replace(/(?:\r\n|\r|\n)/g, '<br>');
    document.getElementById("commandOutput").innerHTML += string;
});
