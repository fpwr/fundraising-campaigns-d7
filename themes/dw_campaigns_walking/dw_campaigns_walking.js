(function($) {
    dw_campaigns_derby = {
        
        /**
         * init cancer
         */
        init: function() {
                var $ = jQuery; 
/*
                if($.browser.msie && $.browser.version <= 7.0) {            
       			$('body:not(.dw-user-registeross, .donation-page, .dw-user-donations-add, .dw-user-register, .dw-user-profile, .walking-home, .walking-location, .dw-walking-host-create, .page-dw-offline, .page-dw-user-host-users) select,').parent('*').jqTransform();        
		} else {
			var isiPad = navigator.userAgent.match(/iPad/i) != null;
			if(!isiPad) {
       				$('body:not(.dw-user-registeross, .donation-page, .dw-user-donations-add, .dw-user-register, .dw-user-profile, .dw-walking-host-create, .page-dw-offline, .page-dw-user-host-users) select, body.donation-page .walking-header-bottom select').parent('*').jqTransform();        
			}
		}
*/
        	/**
        	 * Error div
        	 */
        	$('#console .limiter div:first').hide()
        						  .fadeIn()
        						  .center()
        					      .append('<a href="#" class="close">X</a>')
        					      .click(function () {
        					      		$(this).fadeOut();
        					      });
        }
    };  

    $(document).ready(function(e) {
        dw_campaigns_derby.init();
    });


})(jQuery);



jQuery.fn.center = function () {
    var $ = jQuery; 

    this.css("position","absolute");
    this.css("top", ( $(window).height() - this.height() ) / 2+$(window).scrollTop() + "px");
    this.css("left", ( $(window).width() - this.width() ) / 2+$(window).scrollLeft() + "px");
    return this;
}
