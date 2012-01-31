(function ($) {
  CKEDITOR.plugins.add('media_crop_ckeditor', {
    init: function (editor, pluginPath) {

      editor.addCommand('mediaCropEditInstance',{
        exec: function (editor){
          var sel = editor.getSelection();
          var element = sel.getStartElement();
          var instanceId = editor.name;
          var settings = Drupal.settings.wysiwyg.plugins.drupal['media'];
          var attributes = Drupal.wysiwyg.plugins.media.getAttributesFromClass(element.$.className);
          var fid = null;
          var options = {};
          options.crop = {};
          for (var name in attributes) {
            if (name === 'fid') {
              fid = attributes[name].value;
            }
            else {
              options.crop[name] = attributes[name].value;
            }
          }
          var mediaFile = {
            fid: fid
          };

          // Open the media style selector dialog.
          Drupal.media.popups.mediaStyleSelector(mediaFile, function (formattedMedia) {
            Drupal.wysiwyg.plugins.media.insertMediaFile(mediaFile, formattedMedia.type, formattedMedia.html, formattedMedia.options, Drupal.wysiwyg.instances[instanceId]);
          }, options);
        }
      });

      // Create menu item.
      if (editor.addMenuItems) {
        editor.addMenuItems({
          mediaCropEditInstance: {
            label: 'Image Crop Settings',
            command: 'mediaCropEditInstance',
            group: 'mediaCrop',  //have to be added in config
            icon: this.path + 'plugin.png'
          }
        });
      }

      // Add behaviour if context menu opens up.
      if (editor.contextMenu) {
        //function to be run when context menu is displayed
        editor.contextMenu.addListener(function (element, selection) {
          if (element && element.is('img') && element.hasClass('img__view_mode__media_crop') && (element.getAttribute('src') !== '/broken.jpg')) {
            return { mediaCropEditInstance: CKEDITOR.TRISTATE_OFF };
          }
          return null;
        });
      }
    }
  });

  var mediaBrowserOnLoad = Drupal.media.popups.mediaStyleSelector.mediaBrowserOnLoad;
  Drupal.media.popups.mediaStyleSelector.mediaBrowserOnLoad = function (e) {
    if (document.getElementById('mediaStyleSelector').contentWindow.Drupal.media_crop.actions.init && e.data.crop) {
      document.getElementById('mediaStyleSelector').contentWindow.Drupal.media_crop.actions.init(e.data);
    }
  };
})(jQuery);
