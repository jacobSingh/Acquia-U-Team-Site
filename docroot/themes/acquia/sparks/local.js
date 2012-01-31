/*jslint bitwise: true, eqeqeq: true, immed: true, newcap: true, nomen: false,
 onevar: false, plusplus: false, regexp: true, undef: true, white: true, indent: 2
 browser: true */

/*global jQuery: true Drupal: true window: true ThemeBuilder: true */

(function ($) {
  /**
   * Acquia object is created if it doesn't exist.
   */
  Drupal.behaviors.acquia = Drupal.behaviors.acquia || {};
  
  /**
  * Attach handler.
  */
  Drupal.behaviors.acquia = {
    equalCols: true,
    delay: NaN,
    attach: function (context, settings) {
      var addCommentTrigger = $('.comment-add a');
      var addCommentForm = $('#comment-new');
      var themebuilderIsOpen = Drupal.behaviors.acquia.checkThemeBuilderStatus();
      // Add only if the comment form exists on the page.
      if (addCommentForm.length > 0) {
        // Bind click event handlers to all .comment_add links
        addCommentTrigger.bind('click', {scrollTarget: addCommentForm}, Drupal.behaviors.acquia.scrollTo);
      }
      // Run functions that require a slight delay so that the screen can finish painting
      Drupal.behaviors.acquia.delay = setTimeout(Drupal.behaviors.acquia.delayFunctions, 750);
      // Run the following only if the ThemeBuilder is closed
      if (!themebuilderIsOpen) {
        Drupal.behaviors.acquia.enhanceRotatingBanner();
      }
    }
  };
  
  
  /**
   * Run functions that require a slight delay so that the screen can finish painting
   */
  Drupal.behaviors.acquia.delayFunctions = function () {
    // Set all sidebars to the height of their parent container.
    Drupal.behaviors.acquia.equalizeColumns();
  };
  
  /**
   * Scrolls the page to the clicked element's location
   * @param {event} event
   *   The event.
   */
  Drupal.behaviors.acquia.scrollTo = function (event) {
    // Keep the page from jumping to the hash target
    event.preventDefault();
    var toolbar = {};
    toolbar.menuHeight = $('.toolbar-menu', '#toolbar').height() || 0;
    toolbar.drawerHeight = $('.toolbar-drawer', '#toolbar').height() || 0;
    var targetOffset = event.data.scrollTarget.offset().top - toolbar.menuHeight - toolbar.drawerHeight;
    $('html,body').animate({scrollTop: targetOffset}, 1000);
  };
  
  /**
   * Equalizes all .col elements in a stack to the height of the tallest .col
   */
  Drupal.behaviors.acquia.equalizeColumns = function () {
    if (Drupal.behaviors.acquia.equalCols) {
      $('body').once('acquia-equalcols', function() {
        var $stacks = $('.box');
        for (var i = 0; i < $stacks.length; i++) {
          // Get the current stack reference
          var _this = $stacks.eq(i);
          // Instantiate the boxHeight variable
          var boxHeight = 0;
          // Get the columns in the stack
          var cols = _this.find('.tb-height-balance');
          // We don't need to balance the columns if only one column is present
          if (cols.length > 1) {
            for (var j = 0; j < cols.size(); j++) {
              var col = cols.eq(j);
              // Only balance the height if the col is visible
              if (col.css('display') !== 'none' && col.css('visibility') !== 'hidden') {
                var colHeight = col.outerHeight(false);
                if (colHeight > boxHeight) {
                  // If the height of the col is more than previous height, use it
                  boxHeight = colHeight;
                }
              }
            }
          }
          // Only set the min-heights if boxHeight was assigned a real value
          if (boxHeight > 0) {
            cols.css('min-height', boxHeight);
          }
        }
      });
    }
  };
  
  /**
   * Adds a click handler to the layout wrapper of the rotating banner.  Otherwise this element
   * would obsure the link wrapped around the image element
   */
  Drupal.behaviors.acquia.enhanceRotatingBanner = function () {
    var layouts = $('.layout-wrapper', '.rotating-banner');
    var len = layouts.length;
    if (len > 0) {
      while (len--) {
        Drupal.behaviors.acquia.pseudoLink(layouts.eq(len));
      }
    }
  };
  
  /**
   * Navigates to the designated element's data-link attribute value
   * 
   * @param {DomElement} element
   *   Element that the pseudo link will be applied to
   *
   * @attribute 
   *   data-link is an element attribute that uses the HTML5 data- pattern to store information in the DOM
   */
  Drupal.behaviors.acquia.pseudoLink = function (element) {
    var link = element.attr('data-link');
    if (link && link.length > 0) {
      //Bind a click event to the slide layout wrapper if it has a link, stored in data-link attribute
      element.click(function (event) {
        window.location = $(event.currentTarget).attr('data-link');
      });
    }
  };
  
  /**
   * Determines if the ThemeBuilder is open
   *
   * @return {Boolean}
   *   returns a boolean that represents the open state of the ThemeBuilder. True equals open.
   */
  Drupal.behaviors.acquia.checkThemeBuilderStatus = function () {   
    return $('body').hasClass('themebuilder') ? true : false;
  };
  
}(jQuery));
