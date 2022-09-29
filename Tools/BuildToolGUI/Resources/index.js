window.addEventListener('DOMContentLoaded', (event) => {
    const thisWindow = Window.this;

    document.getElementById("runGenerate").onclick = function(){
        var result = thisWindow.frame.InvokeCommand("-generate");
    };

    document.getElementById("runClean").onclick = function(){
        thisWindow.frame.InvokeCommand("-clean"); 
    };

    document.getElementById("runBuild").onclick = function(){
        thisWindow.frame.InvokeCommand("-build"); 
    };
});