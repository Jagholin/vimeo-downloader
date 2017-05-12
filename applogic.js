function parseJSON(textJSON) {
    var myObject = JSON.parse(textJSON);

    var vidDef = myObject.videoDefinition;
    var audioDef = myObject.audioDefinition;

    videoUrl.text = vidDef.videoDataUrl;
    videoSegments.text = vidDef.videoNumberOfSegments;
    videoWidth.text = vidDef.videoWidth;
    videoHeight.text = vidDef.videoHeight;
    videoInitSegment.text = vidDef.videoInitSegment;
    videoID.text = vidDef.videoId;

    audioUrl.text = audioDef.audioDataUrl;
    audioSegments.text = audioDef.audioNumberOfSegments;
    audioInitSegment.text = audioDef.audioInitSegment;
    audioID.text = audioDef.audioId;

    actionButton.enabled = false;

    var responseFunction = function() {
        downloadInitiator.initiateDownload(vidDef.videoDataUrl, vidDef.videoNumberOfSegments, vidDef.videoInitSegment,
                                           audioDef.audioDataUrl, audioDef.audioNumberOfSegments, audioDef.audioInitSegment,
                                           saveFileDialog.fileUrl);
    }

    saveFileDialog.onAccepted.connect(responseFunction);
    saveFileDialog.open();
}
