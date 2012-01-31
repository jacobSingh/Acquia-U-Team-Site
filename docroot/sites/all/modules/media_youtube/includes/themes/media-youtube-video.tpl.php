<?php

/**
 * @file media_youtube/includes/themes/media-youtube-video.tpl.php
 *
 * Template file for theme('media_youtube_video').
 *
 * Variables available:
 *  $uri - The uri to the YouTube video, such as youtube://v/xsy7x8c9.
 *  $video_id - The unique identifier of the YouTube video.
 *  $width - The width to render.
 *  $height - The height to render.
 *  $autoplay - If TRUE, then start the player automatically when displaying.
 *  $fullscreen - Whether to allow fullscreen playback.
 *
 * Note that we set the width & height of the outer wrapper manually so that
 * the JS will respect that when resizing later.
 */
?>
<div class="media-youtube-preview-wrapper media-embed-resize" id="<?php print $wrapper_id; ?>">
  <?php print $output; ?>
</div>
