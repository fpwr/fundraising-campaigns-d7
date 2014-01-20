var dw_campaigns = {
	
	/**
	 * Called on every page load
	 */	
	init: function() {
                var $ = jQuery;
		//handle any default text fields
		$('.has-default-text').hasDefaultText();
	},
        /**
         * Handles the map in any blocks
         */
        eventMaps:              {},
        geocoder:               null,
        infoWindows:    [],
        initEventsMap: function(params) {
                var $ = jQuery;
           
                if(this.geocoder == null) {
                        this.geocoder = new google.maps.Geocoder();
                }

                if(!params.zoom) {
                        params.zoom = 11;
                }

                var latlng = new google.maps.LatLng(params.lat, params.long);
                var myOptions = {
                                zoom: params.zoom,
                                center: latlng,
                                mapTypeId: google.maps.MapTypeId.ROADMAP,
                                mapTypeControl: false
                };
                var container = $(params.container)[0];
                if(container.id == '') {
                        alert("You must specify an id for your map.");
                }

                this.eventMaps[container.id] = new google.maps.Map(container, myOptions);
        },

        addMarkerToMap: function(container, location, title, html) {
                var $ = jQuery;

                var me          = this;
                var map         = me.eventMaps[$(container)[0].id];

                var marker = new google.maps.Marker({
                        map:            map, 
                        position:       new google.maps.LatLng(location.lat,location.lng),
                        title:          title
                });

                var infowindow = new google.maps.InfoWindow({
                    content: html
                });

                google.maps.event.addListener(marker, 'click', function() {
                        for(var c=0; c<me.infoWindows.length; c++) {
                                me.infoWindows[c].close();
                        }

                        infowindow.open(map,marker);
                });

                me.infoWindows.push(infowindow);

        },	
	/**
	 * will refresh states when a country is selected
	 */
	selectedState: {},
	initCountrySelect: function(countrySelect, stateSelect) {
                var $	= jQuery;
		var me	= this;
                var key	= stateSelect.replace(/[^:alpha:]/, "").replace(/-/g, '_');

		$(countrySelect).change(function() {

			//remove all state/province options
			stateSelect = $(stateSelect)[0];
		
			//track the selected state so if we are simply refreshing we keep the same one selected
			if(me.selectedState[key] == null) {
				me.selectedState[key] = $(stateSelect).val();
			}

                        

                        if(typeof stateSelect.options == 'undefined') {
				return;
			}
	
			//remove all options
			for(var index = stateSelect.options.length - 1; index >= 0; index --) {
				stateSelect.remove(index);
			}
			
			//drop in temp loading option
			var option 		= document.createElement('option');
			option.text 	= "Loading...";
			option.value	= -1;
			stateSelect.options.add(option);
			stateSelect.disabled = true;
			
			$.ajax({
				dataType: 'json',
				url: '/dw/ajax/statesQuery/' + $(this).val(),
				success: function(results) {
					
					//remove the loading optoins
					stateSelect.remove(0);
					//add in new options
                    if( results ){
			    		for(var index = 0; index < results.length; index++) {
		    				var option 	= document.createElement('option');
	    					option.text 	= results[index].name;
    						option.value	= results[index].value;
						    stateSelect.options.add(option);
					    }

                    }else{
                        //JFN - january 20 2014 0717 - [#bugfix "can't seem to connect to the ajax service that provides this information at times, we'll default to this data set when that does happen for now."]
                        //if we can fix the ajax issue on my local machine, we can remove this if/else block entirely.
                        var options = {
                            "": "- select a state-",
                            "1000": "Alabama",
                            "1001": "Alaska",
                            "1052": "American Samoa",
                            "1002": "Arizona",
                            "1003": "Arkansas",
                            "1060": "Armed Forces Americas",
                            "1059": "Armed Forces Europe",
                            "1061": "Armed Forces Pacific",
                            "1004": "California",
                            "1005": "Colorado",
                            "1006": "Connecticut",
                            "1007": "Delaware",
                            "1050": "District of Columbia",
                            "1008": "Florida",
                            "1009": "Georgia",
                            "1053": "Guam",
                            "1010": "Hawaii",
                            "1011": "Idaho",
                            "1012": "Illinois",
                            "1013": "Indiana",
                            "1014": "Iowa",
                            "1015": "Kansas",
                            "1016": "Kentucky",
                            "1017": "Louisiana",
                            "1018": "Maine",
                            "1019": "Maryland",
                            "1020": "Massachusetts",
                            "1021": "Michigan",
                            "1022": "Minnesota",
                            "1023": "Mississippi",
                            "1024": "Missouri",
                            "1025": "Montana",
                            "1026": "Nebraska",
                            "1027": "Nevada",
                            "1028": "New Hampshire",
                            "1029": "New Jersey",
                            "1030": "New Mexico",
                            "1031": "New York",
                            "1032": "North Carolina",
                            "1033": "North Dakota",
                            "1055": "Northern Mariana Islands",
                            "1034": "Ohio",
                            "1035": "Oklahoma",
                            "1036": "Oregon",
                            "1037": "Pennsylvania",
                            "1056": "Puerto Rico",
                            "1038": "Rhode Island",
                            "1039": "South Carolina",
                            "1040": "South Dakota",
                            "1041": "Tennessee",
                            "1042": "Texas",
                            "1058": "United States Minor Outlying Islands",
                            "1043": "Utah",
                            "1044": "Vermont",
                            "1057": "Virgin Islands",
                            "1045": "Virginia",
                            "1046": "Washington",
                            "1047": "West Virginia",
                            "1048": "Wisconsin",
                            "1049": "Wyoming"
                        };

                        for( var thisOption in options ){
                            var option = document.createElement('option');
                                option.text = options[thisOption];
                                option.value = thisOption;
                                stateSelect.options.add(option);
                        }

                    }

                    $(stateSelect).val(me.selectedState[key]);

                    stateSelect.disabled = false;

				}
			});
			
		});
		
		$(countrySelect).change();
	},
    
	initColorPickers: function() {
                var $ = jQuery;
		$('.color1-picker').ColorPicker({
			flat:       true,
			color: $("#edit-color1").val(),
			onChange: function(hsb, hex, rgb, el) {
				$("#edit-color1").val(hex);
				$(".team-logo .inner").css('background-color','#' + hex);
			}
		});
		
		$('.color2-picker').ColorPicker({
			flat: true,
			color: $("#edit-color2").val(),
			onChange: function(hsb, hex, rgb, el) {
				$("#edit-color2").val(hex);
				$(".team-logo").css('background-color','#' + hex);
			}
		});
	},
	personalTypedInFlag: null,
	initIntroText: function(characterCount, introText) {
                var $ = jQuery;
		var me = this;
		$(introText).keydown(function(event) {

			var text = $(this).val();
			var length = text.length;
			var code = (event.keyCode ? event.keyCode : event.which);

			// let movement and delete chars thru always
			if(length >= __MAX_CHARS__) {
				if( code == 8 || code == 46 || code == 38 || code == 39 || code == 40 || code == 41) {
					return true;
				}
				return false;
			}
			return true;
		});

                $(introText).keyup(function() {
                        if($(this).val().length > __MAX_CHARS__) {
                                $(introText).val($(introText).val().substr(0, __MAX_CHARS__));
                        }
                        $(characterCount).html($(this).val().length + ' / __MAX_CHARS__ characters used');
                });

		// fire once to init
		$(introText).keydown();
		$(introText).keyup();

	},
	initLivePosition: function(pcp_id, amountSource, destination) {
                var $ = jQuery;
		var request = null;
		$(amountSource).keyup(function() {

			var amount = $(amountSource).val();
			if(amount.length == 0) {
                            amount = '0';
                        }
                        if(request) {
                            request.abort();
                        }

                        request = $.get('/dw/ajax/position/' + pcp_id + '/' + amount, function(data) {
                            $(destination).html(data);
                        });

		});
		$(amountSource).keyup();
	},
	initRegistrationLocation: function(locationField, target, emptyWords) {
                var $ = jQuery;
		var request = null;

		$(locationField).change(function() {
                        var $ = jQuery;

                        if(request) {
                            request.abort();
                        }

			var location_id = $(locationField).val();
			if(location_id.length == 0) {
				$(target).hide();
				$(emptyWords).show();
                        } else {
                        	request =	$.get('/dw/ajax/registrationlocation/' + location_id, 
							function(data) {
								$(emptyWords).hide();
                            					$(target).html(data); 
								$(target).show();
							}
						);
			}

		});
		$(locationField).change();
	}
};

/**
 * Plugin to require unique username before a form is submitted
 */
(function($) {
	
	$.fn.hasDefaultText = function(){
		return this.each(function(){
			var default_value = $(this).val();
			$(this).focus(function(){
				if ($(this).val() == default_value) {
					$(this).val("");
					$(this).addClass('has-focus');
				}
			});
			$(this).blur(function(){
				if ($(this).val() == "") {
					$(this).val(default_value);
					$(this).removeClass('has-focus');
				}
				
			});
		});
	};
	
	$.fn.extend({
		
		requireUniqueUsername: function() {
			this.each(function() {
				
				var input = $(this);
				
				/**
				 * Wrap div in container to help place progress
				 */
				var wrapper = $(input).wrap('<div class="username-unique-wrapper"></div>');
				
				/**
				 * Drop in progress gif div first so we can hide or show
				 */
				var progress = $('<div class="username-unique-progress"></div>').insertAfter(input);
				$(progress).hide();
				
				/**
				 * Drop in results div
				 */
				var resultsContainer = $('<div class="username-unique-results"></div>').insertAfter($(input).parent("div:first"));
				
				
				/**
				 * search function
				 */
				var request = null;
				var search = function(username) {
					
					/**
					 * Stop any previously running reqests
					 */
					if(request) {
						request.abort();
					}
					
					$(progress).show();
					
					/**
					 * Search
					 */
					request = $.ajax({
						url: '/dw/ajax/username-search',
						type: 'post',
						data: 'username=' + username,
						success: function(results) {

							results_split = results.split('|', 2);

							$(input).val(results_split[1]); // set safe value
							$('.username_insert').html(results_split[1]);
							$(progress).hide();
							$(resultsContainer).removeClass('username-found').removeClass('username-not-found');
							
							/**
							 * Do not let them select a username we found from our search
							 */
							if(results_split[0] == 'found') {
								$(resultsContainer).addClass('username-found').html("Username is already taken.");
								disableSubmit();
							} else {
								$(resultsContainer).addClass('username-not-found').html("Username is available.");
								enableSubmit();
							}
						}
					});
				};
				
				var submitButton 	= $(input).parents("form:first").find("input[type='submit']");
				var submitEnabled 	= false;
				var disableSubmit = function() {
					$(submitButton).css('opacity',0.5);
					submitEnabled = false;
				};
				
				var enableSubmit = function() {
					$(submitButton).css('opacity',1);
					submitEnabled = true;
				};
				
				$(input).parents("form:first").submit(function() {
                                        if(!submitEnabled) {
                                            alert('You must supply a unique username to submit this form - if you registered in a previous year please login or request a new password by clicking the RED login link near the top of this page');
                                        }
					return submitEnabled;
				});
				
				/**
				 * Attach listener to actual input
				 */
				$(input).keyup(function() {
					search($(this).val());
				});
				
				/**
				 * Disable submit button to start
				 */
				disableSubmit();
				
				/**
				 * if the username field has a value, verify it
				 */
				if($(input).val().length > 0) {
					search($(input).val());
				}
				
			});
		},
		assignPersonalURL: function() {
			this.each(function() {
				
				var input = $(this);
				
				/**
				 * search function
				 */
				var request = null;
				var convert = function(userinput) {
					if(dw_campaigns.personalTypedIn) {
						return;
					}
					
					/**
					 * Stop any previously running reqests
					 */
					if(request) {
						request.abort();
					}
					
					/**
					 * Search
					 */
					request = $.ajax({
						url: '/dw/ajax/makeURL',
						type: 'post',
						data: 'name=' + userinput,
						success: function(results) {
							$('#edit-username').val(results);
							$('#edit-username').keyup();
						}
					});
				};
				
				
				/**
				 * Attach listener to actual input
				 */
				$(input).keyup(function() {
					convert($(this).val());
				});
				

				if($(input).val().length > 0) {
					convert($(input).val());
				}
				
			});
		}
		
	});
	
	
	
})(jQuery);

(function($) {
$(document).ready(function(){ 
	dw_campaigns.init();

        $(".add_to_share").click(function(){
            var t = $("#edit-invitation-targets").val();
            var u = this.href.replace("mailto:", "");
            if(t.length>0) {
                t = t + "\n";
            }
            t = t + u;
            $("#edit-invitation-targets").val(t);
            
            $(this).parents('td:first').html('<div class="pending" title="Email added to Message Recipients list - Pending"><span class="pending">Email Pending</span></div>');
            return false;
        });

        $(".fb-close").click(function(){
            parent.jQuery.fancybox.close();
        });

        $(".fb-unhide").click(function(){
            $(".participation-words").hide();
            $(".buttons").hide();
            $("#edit-event-participants").show();
        });

	// debugging
	// $("#show-words").fancybox().trigger("click");	

        $("#show-words").fancybox({
		transitionIn: 'elastic',
		transitionOut: 'elastic',
		type : 'inline',
		height: 480,
		width: 480
	});

        $("a.fb_login").fancybox({
		transitionIn: 'elastic',
		transitionOut: 'elastic',
		type : 'iframe',
		height:260,
                width:380
	});

        $("a.fb").fancybox({
		transitionIn: 'elastic',
		transitionOut: 'elastic',
		type : 'iframe',
		height: 480
	});

        $("a.fb_ns").fancybox({
		transitionIn: 'elastic',
		transitionOut: 'elastic',
                scrolling : 'no',
		type : 'iframe',
		height: 480
	});
        $("a.fb_medium").fancybox({
		transitionIn: 'elastic',
		transitionOut: 'elastic',
		type : 'iframe',
		height: 540
	});
        $("a.fb_tall").fancybox({
		transitionIn: 'elastic',
		transitionOut: 'elastic',
		type : 'iframe',
		height: 710,
                width: 700
	});
});
})(jQuery);

//donations

(function(global) {
	var noop = function(){};
	if ( !global['console'] ) {
		global.console = {
			log : noop,
			debug : noop,
			info : noop,
			warn : noop,
			error : noop,
			assert : noop,
			clear : noop,
			dir : noop,
			dirxml : noop,
			trace : noop,
			group : noop,
			groupCollapsed : noop,
			groupEnd : noop,
			time: noop,
			timeEnd: noop,
			profile: noop,
			profileEnd: noop,
			count: noop,
			exception: noop,
			table: noop
		}
	}
})(window);
