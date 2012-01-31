(function ($) {
  Drupal.media_crop = Drupal.media_crop || {};
  Drupal.media_crop.actions = Drupal.media_crop.actions || {};

  Drupal.behaviors.media_crop = {
    attach: function (context) {
      var freeze = false;

      var container = $('div.media-item', context);
      var jq = parent.jQuery;

      var origWidth;
      var origHeight;
      var imageScale;

      var cropEnabled = false;
      var scaleEnabled = false;
      var imageStyleSafe = (Drupal.settings.media_crop.styles[($('select[name=image_style]', context).val())].safe || false);
      var imageStyleHasCrop = (imageStyleSafe && (Drupal.settings.media_crop.styles[($('select[name=image_style]', context).val())].crop || false));
      var aspectRatioUnlocked = true;

      jq('iframe#mediaStyleSelector').height('900px');

      if (!imageStyleSafe) {
        $('.image-style-description').show();
      }

      var safeImageStyleCropSettings = {
        x: 0,
        y: 0,
        w: 0,
        h: 0
      };

      var getImageScale = function () {
        return 350 / Math.max(origWidth, origHeight);
      };

      var calculateAnchors = function (anchor, orig, crop) {
        var ret = anchor;
        switch (anchor) {
          case "top":
          case "left":
            ret = 0;
            break;
          case "bottom":
          case "right":
            ret = orig - crop;
            break;
          case "center":
            ret = (orig / 2) - (crop / 2);
            break;
        }

        return ret;
      };

      var areaSelectOptions = {
        handles: true,
        onSelectChange: function (img, selection) {
          $('input[name=x]', context).val(selection.x1);
          $('input[name=y]', context).val(selection.y1);
          $('input[name=width]', context).val(selection.width);
          $('input[name=height]', context).val(selection.height);
          updateUICropDimensions(selection.width, selection.height);
          toggleCropOverrideDescription();
        }
      };

      var setMargins = function (img) {
        var w = img.width();
        var h = img.height();

        // Resize the image if it is not in the correct size
        if (w != 350 || h != 350) {
          var normalizer = Math.max(w, h);
          w = Math.round(w / normalizer * 350);
          h = Math.round(h / normalizer * 350);
          img.width(w);
          img.height(h);
        }

        // add left margin for the image
        if (w < 350) {
          img.css('margin-left', (350 - w) / 2);
        }
      };

      // This killswitch is needed because of Internet Explorer.
      // For some weird reason, Internet Explorer's favourite hobby is to
      // run the onLoad event handler. This killswitch makes sure that it
      // runs exactly once.
      var killswitch = false;
      var img = $('div.media-item img');
      if (!killswitch) {
        // Determining the original width and height of the image.
        $('<img/>')
          .load(function () {
            origWidth = this.width;
            origHeight = this.height;
            imageScale = getImageScale();
            resetDimensions();
            setMargins(img);
            updateRotation(0, null);
            killswitch = true;
          })
          .attr('src', $(img).attr('src'));
      }

      $('div.rotated-images img.rotated')
        .load(function () {
          setMargins($(this));
        });

      var resetDimensions = function () {
        $('input[name=x]', context).val('');
        $('input[name=y]', context).val('');
        $('input[name=width]', context).val('');
        $('input[name=height]', context).val('');
        $('div.crop-dimensions div.crop-width .value', context).html(origWidth);
        $('div.crop-dimensions div.crop-height .value', context).html(origHeight);
      };

      var resetScale = function () {
        $('input[name=scale_width]', context).val('');
        $('input[name=scale_height]', context).val('');
      };

      var updateUICropDimensions = function(width, height) {
        $('div.crop-dimensions div.crop-width .value', context).html(Math.round(width / imageScale) + ' px');
        $('div.crop-dimensions div.crop-height .value', context).html(Math.round(height / imageScale) + ' px');
      };

      var updateRotation = function (angle, finished) {
        freeze = true;
        var rot_input = $('input[name=rotate]', context);
        var rot = Number(rot_input.val());
        var oldrot = rot;
        var rotatedImage;

        rot = (rot + angle) % 360;
        if (rot < 0) {
          rot = 360 + rot;
        }

        rot_input.val(rot);

        var rotDiff = Math.abs(oldrot - rot);

        rotatedImage = $('img.rotated-' + rot, context);
        var currentimage = rotatedImage.length > 0 ? rotatedImage : img;

        var animationFinished = function () {
          img
            .show()
            .attr('class', img
              .attr('class')
              .replace(/\bimage-rotate-[-0-9]+-[-0-9]+\b/, ''))
            .addClass('image-rotate-' + oldrot + '-' + rot)
            .imgAreaSelect({hide: true, remove: true});
        };

        var animationOptions = {
          duration: 50,
          easing: 'linear',
          queue: false
        };

        setMargins(currentimage);
        var cih = currentimage.height();

        if (rotDiff) {
          // Resizing the container
          currentimage.animate({
            'margin-top': (350 - cih) / 2
          }, $.extend({
            complete: function () {
              img.css('margin-top', (350 - img.height()) / 2);
            }
          }, animationOptions));
          container.animate({
            height: 350
          }, animationOptions);
          setTimeout(animationFinished, animationOptions.duration);
        }

        // Saves the selection
        var x = Number($('input[name=x]', context).val());
        var y = Number($('input[name=y]', context).val());
        var w = Number($('input[name=width]', context).val());
        var h = Number($('input[name=height]', context).val());

        if (rotDiff) {
          // Moves back the rotated images.
          $('div.media-item img.rotated', context)
            .imgAreaSelect({hide: true, remove: true})
            .appendTo($('div.rotated-images'));
        }

        setTimeout(function () {
          if (rot != 0 && rotDiff) {
            // Replaces the original image with a rotated.
            img
              .hide()
              .after(rotatedImage);
          }

          var selectionData = {};
          var show = false;

          if (x > 0 || y > 0 || w > 0 || h > 0) {
            var iw = currentimage.width();
            var ih = currentimage.height();
            var k, x_, y_, w_, h_;
            switch (rotDiff) {
              case 270:
              case 90:
                // Restores the selection
                k = angle / 90; // k will be either -1 or 1
                if (rotDiff == 270) {
                  k /= 3;
                }
                x_ = k > 0 ? (iw - (h + y)) : y;
                y_ = k > 0 ? x : (ih - (w + x));
                w_ = h;
                h_ = w;
                break;
              case 0:
                x_ = x;
                y_ = y;
                w_ = w;
                h_ = h;
                break;
            }

            selectionData = {
              x1: x_,
              x2: x_ + w_,
              y1: y_,
              y2: y_ + h_
            };

            show = true;
          }

          if (!show) {
            freeze = false;
          }

          container.animate({
            height: cih
          }, animationOptions);
          currentimage.animate({
            'margin-top': 0
          }, animationOptions);

          setTimeout(function () {
            currentimage.imgAreaSelect(
              $.extend({}, areaSelectOptions,
                       {show: show, enable: cropEnabled}, selectionData));
            if (show) {
              $('input[name=x]').val(x_);
              $('input[name=y]').val(y_);
              $('input[name=width]').val(w_);
              $('input[name=height]').val(h_);
              if (!cropEnabled) {
                $('.crop-button').click();
              }
              updateUICropDimensions(w_, h_);
            }
            freeze = false;
            if (finished) {
              finished();
            }
          }, rotDiff ? 500 : 0);

        }, rotDiff ? 350 : 0);
      };

      Drupal.media_crop.actions.updateRotation = updateRotation;

      $('select[name=image_style]', context).change(function () {
        var selected = $(this).val();

        var resetUI = function () {
          resetDimensions();
          updateRotation(360 - Number($('input[name=rotate]').val()), null);

          if (cropEnabled) {
            $('.crop-active-button').click();
          }

          if (scaleEnabled) {
            $('.scale-active-button').click();
          }
        };

        try {
          if (Drupal.settings.media_crop.styles[selected].safe) {
            imageStyleSafe = true;
            $('.image-style-description').hide();
            var props = Drupal.settings.media_crop.styles[selected];
            var oldRot = Number($('input[name=rotate]').val());
            var newRot = Number(props.rotation);
            var w = ((oldRot + newRot) / 90) & 1 ? img.height() : img.width();
            var h = ((oldRot + newRot) / 90) & 1 ? img.width() : img.height();

            resetDimensions();

            updateRotation(newRot, function () {
              if (props.crop) {
                var anchor = props.crop.anchor.split('-');

                imageStyleHasCrop = true;
                safeImageStyleCropSettings = {
                  x: Math.round(calculateAnchors(anchor[0], w, props.crop.width)),
                  y: Math.round(calculateAnchors(anchor[1], h, props.crop.height)),
                  w: Math.round(props.crop.width),
                  h: Math.round(props.crop.height)
                };

                $('input[name=x]', context).val(safeImageStyleCropSettings.x);
                $('input[name=y]', context).val(safeImageStyleCropSettings.y);
                $('input[name=width]', context).val(safeImageStyleCropSettings.w);
                $('input[name=height]', context).val(safeImageStyleCropSettings.h);

                updateRotation(0, toggleCropOverrideDescription());
              }
              else {
                imageStyleHasCrop = false;
              }
            });
          }
          else {
            $('.image-style-description').show();
            imageStyleSafe = false;
            resetUI();
          }
        }
        catch (e) {
          imageStyleSafe = false;
          resetUI();
        }
      });

      $('body').delegate('div.imgareaselect-outer', 'click', function () {
        if (cropEnabled) {
          resetDimensions();
        }
      });

      $('.rotate-left-button:not(.processed)')
        .addClass('processed')
        .click(function () {
           if (!freeze) {
             updateRotation(-90, null);
           }
        });

      $('.rotate-right-button:not(.processed)')
        .addClass('processed')
        .click(function () {
           if (!freeze) {
             updateRotation(90, null);
           }
        });

      $('.crop-button:not(.processed)')
        .addClass('processed')
        .click(function () {
          cropEnabled = true;

          $(this).hide();
          $('.crop-active-button').show();
          $('.crop-description').show();
          $('.crop-dimensions-container').show();
          toggleCropOverrideDescription();
          $('.media-item img:visible')
              .imgAreaSelect({enable: true});
        });

      $('.crop-active-button:not(.processed)')
        .addClass('processed')
        .click(function () {
          cropEnabled = false;

          $(this).hide();
          $('.crop-button').show();
          $('.crop-description').hide();
          $('.crop-dimensions-container').hide();
          $('.crop-aspect-ratio-lock').show();
          $('.crop-aspect-ratio-unlock').hide();
          toggleCropOverrideDescription();
          $('.media-item img:visible')
              .imgAreaSelect({enable: false, hide: true, aspectRatio: false});
          resetDimensions();
        });

      $('.crop-aspect-ratio-lock:not(.processed)')
        .addClass('processed')
        .click(function () {
          var aspRatW = Number($('input[name=width]', context).val());
          var aspRatH = Number($('input[name=height]', context).val());

          if (aspRatW && aspRatH) {
            $(this).hide();
            aspectRatioUnlocked = false;
            $('.crop-aspect-ratio-unlock').show();
            var aspRat = aspRatW + ':' + aspRatH;
            $('.media-item img:visible')
                .imgAreaSelect({aspectRatio: aspRat});
          }
        });

      $('.crop-aspect-ratio-unlock:not(.processed)')
        .addClass('processed')
        .click(function () {
          aspectRatioUnlocked = true;

          $(this).hide();
          $('.crop-aspect-ratio-lock').show();
          $('.media-item img:visible')
              .imgAreaSelect({aspectRatio: false});
        });


        $('.scale-button:not(.processed)')
          .addClass('processed')
          .click(function () {
            scaleEnabled = true;

            $(this).hide();
            $('.scale-active-button').show();
            $('.scale-image-description').show();
            $('.scale-image-container').show();
          });

        $('.scale-active-button:not(.processed)')
          .addClass('processed')
          .click(function () {
            scaleEnabled = false;

            $(this).hide();
            $('.scale-button').show();
            $('.scale-image-description').hide();
            $('.scale-image-container').hide();
            resetScale();
          });

      var toggleCropOverrideDescription = function () {
        if (imageStyleSafe && imageStyleHasCrop && isCropOverridden()) {
          $('.crop-override-description').show();
        }
        else {
          $('.crop-override-description').hide();
        }
      };

      var isCropOverridden = function () {
        var w = Number($('input[name=width]', context).val());
        var h = Number($('input[name=height]', context).val());

        return (
          safeImageStyleCropSettings.w !== w ||
          safeImageStyleCropSettings.h !== h ||
          !cropEnabled
        );
      };

      // Store original Drupal.media.formatForm.getOptions() for extending.
      var getOptions = Drupal.media.formatForm.getOptions;

      // Extend Drupal.media.formatForm.getOptions() to incorporate media_crop
      // related settings.
      Drupal.media.formatForm.getOptions = function () {
        var options = getOptions();

        var mediaCropSettings = {
          media_crop_rotate: Number(($('input[name=rotate]', context).val() || '0')),
          media_crop_x: Number(($('input[name=x]', context).val() || '0')),
          media_crop_y: Number(($('input[name=y]', context).val() || '0')),
          media_crop_w: Number(($('input[name=width]', context).val() || '0')),
          media_crop_h: Number(($('input[name=height]', context).val() || '0')),
          media_crop_scale_w: Number(($('input[name=scale_width]', context).val() || '0')),
          media_crop_scale_h: Number(($('input[name=scale_height]', context).val() || '0')),
          media_crop_image_style: ($('select[name=image_style] option:selected', context).val() || ''),
          media_crop_instance: '%7BMCIID%7D'
        };

        return $.extend({}, options, mediaCropSettings);
      };

      // Store original Drupal.media.formatForm.getFormattedMedia(),
      // if extending it is needed instead of overriding.
      var getFormattedMedia = Drupal.media.formatForm.getFormattedMedia;
      // Override Drupal.media.formatForm.getFormattedMedia(),
      // it is safe to do, as this js gets loaded only for local images.

      Drupal.media.formatForm.getFormattedMedia = function () {
        var formatType = ($('input[name=format]', context).val() || 'media_crop');
        var options = Drupal.media.formatForm.getOptions();
        var token = ($('input[name=token]', context).val() || ' ');
        var template = ($('input[name=template]', context).val() || '');
        var fid = ($('input[name=fid]', context).val() || '0');

        var r = Math.random() * 10000000000000;
        var id = 'media_crop_' + String(r - (r % 1));

        var replacedData = {
          id: id,
          token: token,
          options: options,
          fid: fid
        };

        parent.window.Drupal.media_crop.replaceImage(replacedData);

        var mediadata = {
          type: formatType,
          options: options,
          html: Drupal.settings.media_crop.imageStyleHtml[options.media_crop_image_style]
        };

        mediadata.html = template.replace('ID_PLACEHOLDER', id);

        return mediadata;
      };

      // TODO: check if this function is needed at all
      var getStyleName = function (formatter) {
        if (Drupal.settings.media_crop.styles[formatter]) {
          return Drupal.settings.media_crop.styles[formatter].name;
        }
        return formatter;
      };
    }
  };

  Drupal.media_crop.actions.init = function (data) {
    var crop = data.crop;

    var setCropRotationScale = function () {
      if (Number(crop.media_crop_w) && Number(crop.media_crop_h) && Number(crop.media_crop_rotate)) {
        Drupal.media_crop.actions.updateRotation(Number(crop.media_crop_rotate), function () {
          $('input[name=x]').val(crop.media_crop_x);
          $('input[name=y]').val(crop.media_crop_y);
          $('input[name=width]').val(crop.media_crop_w);
          $('input[name=height]').val(crop.media_crop_h);

          var selectionData = {
            x1: Number(crop.media_crop_x),
            x2: Number(crop.media_crop_x) + Number(crop.media_crop_w),
            y1: Number(crop.media_crop_y),
            y2: Number(crop.media_crop_y) + Number(crop.media_crop_h)
          };

          Drupal.media_crop.actions.updateRotation(0, null);
        });
      }
      else if (Number(crop.media_crop_w) && Number(crop.media_crop_h)) {
        $('input[name=x]').val(crop.media_crop_x);
        $('input[name=y]').val(crop.media_crop_y);
        $('input[name=width]').val(crop.media_crop_w);
        $('input[name=height]').val(crop.media_crop_h);

        var selectionData = {
          x1: Number(crop.media_crop_x),
          x2: Number(crop.media_crop_x) + Number(crop.media_crop_w),
          y1: Number(crop.media_crop_y),
          y2: Number(crop.media_crop_y) + Number(crop.media_crop_h)
        };

        Drupal.media_crop.actions.updateRotation(0, null);
      }
      else if (Number(crop.media_crop_rotate)) {
        Drupal.media_crop.actions.updateRotation(Number(crop.media_crop_rotate), null);
      }

      if (Number(crop.media_crop_scale_w) || Number(crop.media_crop_scale_h)) {
        $('.scale-button').click();
        $('input[name=scale_width]').val(crop.media_crop_scale_w);
        $('input[name=scale_height]').val(crop.media_crop_scale_h);
      }
    };

    $('select[name=image_style]').val(crop.media_crop_image_style);
    $('select[name=image_style]').change();

    if (Drupal.settings.media_crop.styles[crop.media_crop_image_style] &&
        Drupal.settings.media_crop.styles[crop.media_crop_image_style].safe) {
      Drupal.media_crop.actions.updateRotation(360 - Number($('input[name=rotate]').val()), setCropRotationScale());
    }
    else {
      setCropRotationScale();
    }

  };
})(jQuery);
