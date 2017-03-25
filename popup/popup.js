window.onload = function() { 
	document.getElementById('life-form').addEventListener('submit', createLife)
}

const createLife = function(event) {
	event.preventDefault()
	chrome.tabs.executeScript(null, {file: "content.js"});
};

const restorePage = function(event) {
	event.preventDefault();
}