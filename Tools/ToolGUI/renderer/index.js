window.addEventListener('DOMContentLoaded', () => {

    window.electronAPI.onLogClear((event) => 
    {
        console.log("clear log");
        document.getElementById('commandOutput').innerHTML = "";
    });

    window.electronAPI.onLogAppend((event, value) => 
    {
        document.getElementById('commandOutput').innerHTML += value;
    });

    document.getElementById('runGenerate').addEventListener('click', async () => {
        window.electronAPI.runBuildTool("-generate");
    });

    document.getElementById('runClean').addEventListener('click', async () => {
        window.electronAPI.runBuildTool("-clean");
    });

    document.getElementById('runBuild').addEventListener('click', async () => {
        window.electronAPI.runBuildTool("-build");
    });
});
