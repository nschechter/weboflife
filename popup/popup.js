window.onload = function() { 
	document.getElementById('life-form').addEventListener('submit', createLife)
}

const createLife = function(event) {
	event.preventDefault()
	console.log(document.getElementsByTagName("input")[0].value)
	chrome.tabs.executeScript(null, {file: "content.js"});
};