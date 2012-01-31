// $Id: vertical_accordion.js,v 1.2 2010/12/21 15:54:27 jameselliott Exp $

(function ($) {

/**
 * Call jQuery Accordion on vertical_accordion elements
 */
Drupal.behaviors.verticalAccordion = {
  attach: function (context) {
    $('.vertical-accordion', context).once('vertical-accordion', function(){
      $(this).accordion({
        header: '.pane-header',
        clearStyle: true,
        autoHeight: false
      });
    });
  }
};

})(jQuery);
