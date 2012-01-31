// $Id$
(function ($) {

/**
 * Hide and reveal the content of a node contained within a simpleview.
 */
Drupal.behaviors.sqbsRevealNodeContent = {
  attach: function (context, settings) {
    // Find nodes in the relevant simpleview.
    $('.simpleview-title-reveals-full .node', context).once('sqbs-reveal-node-content', function () {
      var title = $(this).find('h2 a');
      var content = $(this).find('.content');
      // Hide the contents of the node, then toggle its visibility when the
      // node title is clicked.
      content.hide();
      title.click(function (e) {
        if (content.is(":hidden")) {
          content.show();
        }
        else {
          content.hide();
        }
        // Don't follow the link.
        e.preventDefault();
      });
    });
  }
};

})(jQuery);
