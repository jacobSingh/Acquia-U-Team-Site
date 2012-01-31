
(function( $ ){

  Drupal.behaviors.media_embed = {
    attach: function (context, settings) {
      // Fluidly size embedded media within wrappers containing the
      // 'media-embed-resize' class to fit in their containing block element.
      // This logic is based on http://fitvidsjs.com/.
      $('.media-embed-resize', context).find('object, embed, iframe').each(function(i, element) {
        var $this = $(this),
            height = this.tagName == 'OBJECT' ? $this.attr('height') : $this.height(),
            aspectRatio = height / $this.width();

        var dimensions = {
          'max-height': height,
          'max-width': $this.width()
        };

        $this
          .wrap('<div class="media-embed-wrapper" />').parent().css({'padding-top' : (aspectRatio * 100)+"%"})
          .wrap('<div class="media-embed-wrapper-outer" />').parent().css(dimensions);
        $this.removeAttr('height').removeAttr('width');
      });
    }
  }

})( jQuery );
