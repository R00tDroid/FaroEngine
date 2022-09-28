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

    const btn = document.getElementById('runGenerate');
    btn.addEventListener('click', async () => {
        const result = await window.electronAPI.runBuildTool();
        console.log("command result: " + result);
        document.getElementById('commandOutput').innerHTML += result;
    });
});
