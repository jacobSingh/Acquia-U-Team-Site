<?php

/**
 *  @file
 *  Create a YouTube Stream Wrapper class for the Media/Resource module.
 */

/**
 *  Create an instance like this:
 *  $youtube = new ResourceYouTubeStreamWrapper('youtube://?v=[video-code]');
 */
class MediaYouTubeStreamWrapper extends MediaReadOnlyStreamWrapper {
  protected $base_url = 'http://youtube.com/watch';

  function getTarget($f) {
    return FALSE;
  }

  static function getMimeType($uri, $mapping = NULL) {
    return 'video/youtube';
  }

  function getOriginalThumbnailPath() {
    $parts = $this->get_parameters();
    return 'http://img.youtube.com/vi/'. check_plain($parts['v']) .'/0.jpg';
  }

  function getLocalThumbnailPath($create = TRUE) {
    $parts = $this->get_parameters();
    $local_path = 'public://media-youtube/' . check_plain($parts['v']) . '.jpg';
    if ($create && !file_exists($local_path)) {
      $dirname = drupal_dirname($local_path);
      file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
      @copy($this->getOriginalThumbnailPath(), $local_path);
    }
    return $local_path;
  }

  protected function getImageStyleURI($uri) {
    if ($target = file_uri_target($uri)) {
      $target_parts = explode('/', $target);
      if ($target_parts[0] == 'styles') {
        $style_name = $target_parts[1];
        $v = check_plain($target_parts[4]);
        return "public://styles/$style_name/public/media-youtube/$v.jpg";
      }
    }
  }

  public function url_stat($uri, $flags) {
    if ($image_style_uri = $this->getImageStyleURI($uri)) {
      $instance = file_stream_wrapper_get_instance_by_uri($image_style_uri);
      return $instance->url_stat($image_style_uri, $flags);
    }
    else {
      $stat = parent::url_stat($uri, $flags);
      $stat[2] = $stat['mode'] = 0100444;
      return $stat;
    }
  }

  public function unlink($uri) {
    if ($image_style_uri = $this->getImageStyleURI($uri)) {
      $instance = file_stream_wrapper_get_instance_by_uri($image_style_uri);
      return $instance->unlink($image_style_uri);
    }
    else {
      $instance = file_stream_wrapper_get_instance_by_uri($uri);
      $local_thumbnail = $instance->getLocalThumbnailPath(FALSE);
      if ($local_thumbnail && file_exists($local_thumbnail)) {
        file_unmanaged_delete($local_thumbnail);
      }
      return TRUE;
    }
  }
}
