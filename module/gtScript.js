
// The following accounts have not been moved under the GoodThreads partner account.
if (gtPartnerID != "KOMEN" && gtPartnerID != "TIT" && gtPartnerID != "MRF" && gtPartnerID != "Blackbaud" && gtPartnerID != "BlackBaud" && gtPartnerID != "HMEA")
{
	gtPartnerID = 'GoodThreads';

}
//var gtPartnerID = 'GoodThreads';

<!-- Google Analytics API Initialization-->
var gaJsHost = (("https:" == document.location.protocol) ? "https://ssl." : "http://www.");
document.write(unescape("%3Cscript src='" + gaJsHost + "google-analytics.com/ga.js' type='text/javascript'%3E%3C/script%3E"));		
<!--
// -----------------------------------------------------------------------------
// Globals
// Major version of Flash required
var requiredMajorVersion = 10;
// Minor version of Flash required
var requiredMinorVersion = 0;
// Minor version of Flash required
var requiredRevision = 124;
var feedback_widget;
var asset_host;
var domainURL = "https://cache.audiolife.com";
var webPropertyID = "UA-26035541-1";    //Prod Value
//var webPropertyID = "UA-25988786-1";    //QA Value
// -----------------------------------------------------------------------------
// -->

function addScript(url, type) {
	var oHead = document.getElementsByTagName('HEAD').item(0);
	var oScript ;
	if(type="script") {
		oScript = document.createElement("script");
		oScript.type = "text/javascript";
		oScript.src=url;
	}
	else if(type="css") {
		oHead = document.getElementsByTagName('HEAD').item(0);
		oScript= document.createElement("link");
		oScript.type = "text/css";
		oScript.rel = "stylesheet";
	}
	oScript.src=url;
	oHead.appendChild(oScript);
}


$(document).ready(function() {
	$("BODY").append("<div id='background'></div>");
	$("#GoodThreadsContent").append("<input type='button' id='btnShowPlayer' value='" + gtButtonTitle + "' onclick='onButtonClickLoad();' /><br/>" +
			"<div id='infodiv' style='display:none;'>Alternate HTML content should be placed here. This content requires the Adobe Flash Player. <a href='http://www.adobe.com/go/getflash/'>Get Flash</a></div>" +
			"<div id='poppyDiv' style='overflow:hidden;display:inline;width:910px;height:640px;visibility:none;padding-left:7px;' title='" + gtStoreTitle + "'></div>");
});

function showPopup() {
	$("#poppyDiv").dialog("open");
	$(".ui-dialog-titlebar-close").dialog({ autoOpen: false });
	if($.browser.safari = $.browser.webkit && !window.chrome)
	 {
		$("#poppyDiv").width(910);
		$(".ui-dialog-titlebar").width($("#poppyDiv").width() + 11).css("webkit-border-radius","0px");
		$(".ui-dialog").width($("#poppyDiv").width() + 21).css("webkit-border-radius","0px");
		$(".ui-dialog-content").css("margin-left","-5");
		$(".ui-dialog-titlebar").css("font-size", "11px").css("height", "14px").css("padding-top", "0px").css("margin-left", "-3px").css("margin-right", "-6px").css("margin-top", "-6px").css("margin-bottom","5");
		$(".ui-dialog").height(645);
		//$("ui-dialog.content");
		//$(".ui-dialog").width(910);
	 }
	else if (($.browser.msie)==true)
	 {
		$("#poppyDiv").width(918);
		$(".ui-dialog-titlebar").width($("#poppyDiv").width() + 11);
		$(".ui-dialog").width($("#poppyDiv").width() + 19);
		$(".ui-dialog-content").css("margin-left","-5").css("margin-right","0");
		$(".ui-dialog-titlebar").css("font-size", "11px").css("height", "14px").css("padding-top", "0px").css("margin-left", "-4px").css("margin-right", "-6px").css("margin-top", "-6px").css("margin-bottom","5");
		$(".ui-dialog").height(640);
	} 
	else
	{
		$(".ui-dialog").width($("#poppyDiv").width() + 19).css("border-top-left-radius","0px");
		$(".ui-dialog-titlebar").width($("#poppyDiv").width() + 9);
		$("#poppyDiv").css("margin-left","-6");
		$(".ui-dialog-titlebar").css("font-size", "11px").css("border-radius","0px").css("height", "14px").css("padding-top", "0px").css("margin-left", "-3px").css("margin-top", "-6px").css("margin-bottom","5").css("margin-right","-6");
		$(".ui-dialog").height(644);	
	}
	//$(".ui-dialog").css("top","-5px").css("padding-top","6px");    
	$(".ui-dialog").css("position", "absolute").css("top","25px").css("padding-top","6px");
	$(".ui-dialog").css("border","4px solid #cecece");
	$(".ui-dialog").css("left","40px").css("left", (($(document).width() - $("#poppyDiv").width() - 25)/2) + "px");
	 
	$(".ui-widget-content").mouseover(function() {
		$(this).css('cursor','pointer');
		}).mouseout(function(){
		$(this).css('cursor','auto');
		});
	
	if(gtUserId == 282455) {
		$(".ui-dialog").append("<div style='z-index:2000;' id='closeDiv'>" + 
							  "<a id='popupClose' style='top:-15px; right:-12px; float:right;position:absolute;' onclick='javascript:closePopup();'>" +
									"<img src='https://cache.audiolife.com/widget/PoppyV1/CLOSE_BUTTON_SQUARE.png' alt='close' height='48' width='48' />" + 
                              "</a>" + 
							"</div>");
	}
	else {
		$(".ui-dialog").append("<div style='z-index:2000;' id='closeDiv'>" + 
								  "<a id='popupClose' style='top:-26px; right:-26px; float:right;position:absolute;' onclick='javascript:closePopup();'>" +
										"<img src='https://cache.audiolife.com/widget/PoppyV1/CLOSE_BUTTON.png' alt='close' height='48' width='48' />" + 
								  "</a>" + 
								"</div>");
	}
	$(".ui-dialog").css({
        "-moz-box-shadow" : "0px 0px 35px 0px #000000 ",
	    "-webkit-box-shadow" : "0px 0px 35px 0px #000000 ",
	    "box-shadow" : "0px 0px 35px 0px #000000 ",
		"-ms-box-shadow" : "0px 0px 35px 0px #000000 "
	  }); 					
	$("#background").css({
					"position": "absolute", 
					"opacity" : "0.6", 
					"top" : "0px", 
					"left" : "0px", 
					"height": $(document).height() + "px", 
					"width": $(document).width() + "px",
					"background": "#000000", 
					"z-index": "1"
					}).fadeIn("slow");
	scroll(0, 0);
}

function showStore() {
	if(gtStoreMode == 'inline') {
		$("#btnShowPlayer").hide();
		$("#background").hide();
		if(document.getElementById('infodiv').style.display == 'none')
			$("#poppyDiv").show();
		else
			$("#infodiv").show();
		raisePoppyStoreLaunchEvent();
	}
	else {
		$("#poppyDiv").dialog({ draggable: true, resizable: false, autoOpen: false,
										beforeClose : function(event, ui) {
											$("#background").fadeOut("slow");
									}});
		if(typeof gtAutoOpenStoreInPopup == 'undefined' || gtAutoOpenStoreInPopup == true) {
			showPopup();
			raisePoppyStoreLaunchEvent();
		}
	}
}
	
$(window).load( function(){
	loadPlayer();
	raiseLandingPageEvent();
	console.log(asset_host);
	//addScript(asset_host, "script");
});

function showFeedbackWidget() {
	feedback_widget.show();
}

function loadPlayer() {
	<!--
	// Version check for the Flash Player that has the ability to start Player Product Install (6.0r65)
	var hasProductInstall = DetectFlashVer(6, 0, 65);

	// Version check based upon the values defined in globals
	var hasRequestedVersion = DetectFlashVer(requiredMajorVersion, requiredMinorVersion, requiredRevision);
	if ( hasProductInstall && !hasRequestedVersion ) {
	//if ( true ) {	
		// DO NOT MODIFY THE FOLLOWING FOUR LINES
		// Location visited after installation is complete if installation is required
		var MMPlayerType = (isIE == true) ? "ActiveX" : "PlugIn";
		var MMredirectURL = window.location;
		document.title = document.title.slice(0, 47) + " - Flash Player Installation";
		var MMdoctitle = document.title;
		var alternateContent = 'This content requires the Adobe Flash Player Latest version. '
		+ 'Would you like to install it now?. '
		+ '<a href=http://www.adobe.com/go/getflash/>Get Flash</a>';
		document.write(alternateContent); // insert non-flash content
		AC_FL_RunContent(
			"src", "playerProductInstall",
			"FlashVars", "MMredirectURL="+MMredirectURL+'&MMplayerType='+MMPlayerType+'&MMdoctitle='+MMdoctitle+"",
			"width", "100%",
			"height", "100%",
			"align", "middle",
			"id", "Designer",
			"quality", "high",
			"bgcolor", "#ffffff",
			"name", "Designer",
			"allowScriptAccess","sameDomain",
			"type", "application/x-shockwave-flash",
			"pluginspage", "http://www.adobe.com/go/getflashplayer"
		);
		document.getElementById('poppyDiv').style.display = "none";
		document.getElementById('infodiv').style.display = "block;";
	}
	else if(hasRequestedVersion)
	{
		document.getElementById('infodiv').style.display = "none";
		var path = "";
		if(typeof gtGoodthreadsVersion == 'undefined')
		{
			path = domainURL + "/widget/PoppyV1/";
		}
		else
		{
			path = domainURL + "/widget/PoppyV" + gtGoodthreadsVersion + "//";
		}
		
		$("#poppyDiv").html("<object classid='clsid:D27CDB6E-AE6D-11cf-96B8-444553540000' id='PoppyCustomizeTool' name='PoppyCustomizeTool' width='910' height='625' codebase='//fpdownload.macromedia.com/get/flashplayer/current/swflash.cab'>" +
		                        "<param name='base' value='" + path + "/' />" +
		                        "<param name='movie' value='" + path + "/PoppyCustomizeTool.swf' />" +
		                        "<param name='quality' value='high' />" +
		                        "<param name='bgcolor' value='#ffffff' />" +
		                        "<param name='allowScriptAccess' value='always'/>" +
		                        "<param name='allowFullScreen' value='true'/>" +
		                        "<param name='FlashVars' value='" + buildParameterString() + "' />" +
		                        "<embed src='" + path + "PoppyCustomizeTool.swf'" +
		                            " quality='high' bgcolor='#FFFFFF' width='910px' height='625px' name='PoppyCustomizeTool' align='middle' play='true' loop='false' quality='high' allowScriptAccess='always' " +
		                            " base='" + path + "/' type='application/x-shockwave-flash' pluginspage='http://www.adobe.com/go/getflashplayer' " + 
		                            " flashVars='" + buildParameterString() + "'> </embed>" +
		                    "</object>");
		showStore();
	}
	else {  // flash is too old or we can't detect the plugin
document.getElementById('GoodThreadsContent').style.display = "none";
return;
		var alternateContent = 'Alternate HTML content should be placed here. '
		+ 'This content requires the Adobe Flash Player. '
		+ '<a href=http://www.adobe.com/go/getflash/>Get Flash</a>';
		document.getElementById('poppyDiv').style.display = "none";
		document.getElementById('infodiv').style.display = "block";
		$("#btnShowPlayer").hide();
		
		if(typeof _gat != 'undefined') {
			var pgTracker = _gat._getTracker(webPropertyID);
			pgTracker._trackPageview('/PoppyV1/'+gtPartnerID);
			var partnerDetails;
			if(typeof gtBandName == 'undefined' || gtBandName.length <= 0)
				partnerDetails = gtPartnerID+'-'+gtUserId;
			else
				partnerDetails = gtBandName;
			pgTracker._trackEvent('FlashNotAvailable', 'Flash Not Available', partnerDetails, gtUserId);
		}
		
	}
	// -->

	//var feedback_widget = new GSFN.feedback_widget(feedback_widget_options);
}

function buildParameterString() {
    
    if(typeof gtBlackbaudDonationEventId == 'undefined')
        gtBlackbaudDonationEventId = '';
    
	if(typeof gtPartnerUserID == 'undefined')
        gtPartnerUserID = '';
	
	if(typeof gtStoreLocation == 'undefined')
        gtStoreLocation = '';
		
    //Removing feedback link from all stores for now
    gtShowFeedBackLink = "false";
    
    return "isAdmin=false" + 
            "&partnerID=" + gtPartnerID + 
            "&themeName=" + gtThemeName + 
            "&customizeButtonName=" + gtCustomizeButtonName + 
            "&bandID=" + gtUserId + 
            "&showFeedBackLink=" + gtShowFeedBackLink + 
            "&webPropertyID=" + webPropertyID + 
            "&blackbaudDonationEventId=" + gtBlackbaudDonationEventId + 
			"&gtPartnerUserID=" + gtPartnerUserID + 
			"&gtStoreLocation=" + gtStoreLocation;
}
function closePopup()
{
    $("#PoppyCustomizeTool").hide();
    $(".ui-widget-content").hide();
    $(".ui-dialog").hide();
    $("#poppyDiv").hide();
    $("#background").fadeOut("fast");    
}
function onButtonClickLoad()
{
    $("#background").show();   
    $("#poppyDiv").show();
    $(".ui-dialog").show();
    $(".ui-widget-content").show();
    $("#PoppyCustomizeTool").show();  
	$(".ui-resizable").hide();	
	showPopup();
	raisePoppyStoreLaunchEvent();
}

function raiseLandingPageEvent() {
	if(typeof _gat != 'undefined') {
		var pgTracker = _gat._getTracker(webPropertyID);
		var partnerDetails;
		if(typeof gtBandName == 'undefined' || gtBandName.length <= 0)
			partnerDetails = gtPartnerID+'-'+gtUserId;
		else
			partnerDetails = gtBandName;
		pgTracker._trackPageview('/PoppyV1/'+gtPartnerID);
		pgTracker._trackEvent('LandingPage', 'Landing Page', partnerDetails, gtUserId);
	}
}

function raisePoppyStoreLaunchEvent() {
	if(typeof _gat != 'undefined') {
		var partnerDetails;
		if(typeof gtBandName == 'undefined' || gtBandName.length <= 0)
			partnerDetails = gtPartnerID+'-'+gtUserId;
		else
			partnerDetails = gtBandName;
		var pgTracker = _gat._getTracker(webPropertyID);
		pgTracker._trackEvent('PoppyStoreLaunched', 'Poppy Store Launched', partnerDetails, gtUserId);
	}
}
