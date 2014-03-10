<!-- this page was used for an advertising campaign - this probably should not be used for a more general purpose -->
<style type='text/css' media='all'>
 .location-raised .location-label {
        color: #5DA0CD;
        font-size: 18px;
        font-style:bold;
        left: 120px;
        position: absolute;
        top: 4px;
}

 .have-raised .dollar-label {
        color: #2E2E2E;
        font-size: 18px;
        left: 120px;
        position: absolute;
        text-transform: uppercase;
        top: 55px;
}

 .have-raised .dollar-amount {
        font-size: 18px;
        left: 122px;
        line-height: 47px;
        position: absolute;
        text-align: center;
        top: 82px;
        width: 125px;
}

 .toward-goal .dollar-label {
        color: #70706F;
        font-size: 17px;
        font-weight: normal;
        left: 122px;
        position: absolute;
        top: 139px;
}

 .toward-goal .dollar-amount {
        color: #897B00;
        font-size: 18px;
        left: 122px;
        position: absolute;
        top: 160px;
}

 .thermoEmpty {
        height: 126px;
        left: 30px;
        position: absolute;
        top: 21px;
        width: 60px;
        background-image:url('/sites/all/themes/dw_campaigns_walking/images/thermoEmptyHome.gif');
}

 .thermoFull {
position: relative;
bottom: -34px;
        left: 0px;
        right:0px;
        background:transparent url('/sites/all/themes/dw_campaigns_walking/images/thermoFullHome.gif') no-repeat scroll center bottom;
}

li {
list-style:none;
}

#thermoBlock {
padding: 10px;
position: relative;
height:230px;
width:290px;
background: #E8F1F7 url('/sites/all/themes/dw_campaigns_walking/images/thermoRight.gif') no-repeat scroll 1px 0px;
left:100px;
}

#words {
background-color: #E8F1F7;
padding:10px;
}
</style>
    
<div id="words">
    <p>100% of proceeds from One SMALL Step events go towards funding Prader-Willi Research as described in the Prader-Willi Research plan.  Our goal for 2012 is to raise $1 million dollars for PWS Research.</p><p>Funds raised in 2012 and 2013 will be specifically designated to investigate hyperphagia and mental illness in PWS.  You can learn more about research funded from previous One SMALL Step campaigns <a href="http://onesmallstep.fpwr.org/node/451" target="_blank">here</a>.
    </p>
    <div id="thermoBlock">
        <?php theme('dw_campaigns_walking_statistics', array('campaign' => NULL, 'terse' => FALSE)); ?>
    </div>
</div>
