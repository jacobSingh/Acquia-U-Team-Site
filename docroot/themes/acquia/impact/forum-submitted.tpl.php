<?php

/**
 * @file
 * Local implementation of forum-submitted.tpl.php
 * by whom a topic was submitted.
 *
 * Available variables:
 *
 * - $author: The author of the post.
 * - $time: How long ago the post was created.
 * - $topic: An object with the raw data of the post. Unsafe, be sure
 *   to clean this data before printing.
 *
 * @see template_preprocess_forum_submitted()
 * @see theme_forum_submitted()
 */
?>
<?php if ($time): ?>
  <span class="submitted">
  <?php print t('@time ago <br> by !author', array(
    '@time' => $time,
    '!author' => $author,
    )); ?>
  </span>
<?php else: ?>
  <?php print t('n/a'); ?>
<?php endif; ?>
