// otevře popup s obsahem
function openPopup(title, contentHTML) {
    document.getElementById("popupTitle").innerText = title;
    document.getElementById("popupBody").innerHTML = contentHTML;
    document.getElementById("popup").style.display = "flex";
}

// zavře popup
function closePopup() {
    document.getElementById("popup").style.display = "none";
}