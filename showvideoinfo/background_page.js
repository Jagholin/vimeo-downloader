// This page will run in background to listen to file events

(function() {
    var lastInterceptedVideo = "";

    chrome.webRequest.onCompleted.addListener((details) => {
        if (details.tabId === -1) {
            console.log('Request isn\'t sent by a tab');
            return;
        }

        // We are looking for a file named master.json
        const filenameRegexp = /^(http(?:s)?:\/\/(?:\S|\/)+?\/sep\/video\/)(?:\S|\/)+?master\.json\?\S*$/;
        const urlMatchData = details.url.match(filenameRegexp);
        // if (details.url.search(filenameRegexp) !== -1) {
        if (urlMatchData !== null) {
            console.log(`Expression matched! TabID: ${details.tabId} Url: ${details.url}`);

        // Redo the request and obtain the response document
            //this.onVimeoDetected(urlMatchData[0], urlMatchData[1]);
            lastInterceptedVideo = urlMatchData[0];
        }
    }, { 'urls' : ['<all_urls>'] }, [] );

    chrome.runtime.onMessage.addListener((message, sender, sendResponse) => {
        if (message === 123)
            sendResponse(lastInterceptedVideo);
    })
})()
