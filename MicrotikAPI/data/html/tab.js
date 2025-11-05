function openTab(evt, tabName) {
    var i, tabcontent, tablinks;
    tabcontent = document.getElementsByClassName("config-table");
    for (i = 0; i < tabcontent.length; i++) {
        tabcontent[i].style.display = "none";
    }
    tablinks = document.getElementsByClassName("tab-btn");
    for (i = 0; i < tablinks.length; i++) {
        tablinks[i].className = tablinks[i].className.replace(" active", "");
    }
    document.getElementById(tabName).style.display = "table";
    evt.currentTarget.className += " active";
}



function append_system() {
    get_json_data('info/system.json', function (json) {
        const table = document.getElementById('systemTable');
        table.innerHTML = ''; // vymaž starý obsah

        // projdi všechny klíče a hodnoty
        Object.entries(json).forEach(([key, value]) => {
            const tr = document.createElement('tr');
            tr.innerHTML = `<td>${key}</td><td>${value}</td>`;
            table.appendChild(tr);
        });
    });
}
