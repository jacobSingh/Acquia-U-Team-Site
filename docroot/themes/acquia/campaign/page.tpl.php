<?php

/**
 * @file
 * Local implementation of page.tpl.php:
 * - stack and width are used in tandem to create center-aligned page regions.
 * - deco stacks are used for background imagery.  These are absolutely positioned and render outside the flow of the content
 * - Use wrapper-header, wrapper-content and wrapper-footer to scope styles to these areas
 */
?>

<div id="page" class="page clearfix tb-terminal">

  <div id="page-deco-top" class="deco-page-top deco-page deco-top deco tb-scope tb-scope-prefer">
    <div class="width deco-width inner tb-terminal">
      <div class="layer-a layer"></div>
      <div class="layer-b layer"></div>
      <div class="layer-c layer"></div>
      <div class="layer-d layer"></div>
    </div>
  </div>
  
  <div id="page-deco-bottom" class="deco-page-bottom deco-page deco-bottom deco tb-scope tb-scope-prefer">
    <div class="width deco-width inner tb-terminal">
      <div class="layer-a layer"></div>
      <div class="layer-b layer"></div>
      <div class="layer-c layer"></div>
      <div class="layer-d layer"></div>
    </div>
  </div>
    
  <div class="page-width tb-scope">
    <div class="lining tb-terminal"> <!-- Broken out of .page-width to avoid update issues with margin: 0 auto being undone -->
    
      <div id="header" class="wrapper-header wrapper clearfix tb-scope">
      
        <div id="header-deco-top" class="deco-header-top deco-header deco-top deco tb-scope tb-scope-prefer">
          <div class="width deco-width inner tb-terminal">
            <div class="layer-a layer"></div>
            <div class="layer-b layer"></div>
            <div class="layer-c layer"></div>
            <div class="layer-d layer"></div>
          </div>
        </div>
        
        <div id="header-deco-bottom" class="deco-header-bottom deco-header deco-bottom deco tb-scope tb-scope-prefer">
          <div class="width deco-width inner tb-terminal">
            <div class="layer-a layer"></div>
            <div class="layer-b layer"></div>
            <div class="layer-c layer"></div>
            <div class="layer-d layer"></div>
          </div>
        </div>
      
        <?php if (!empty($page['preheader_first']) || !empty($page['preheader_second'])) : ?>
          <div id="preheader" class="stack-preheader stack-pre stack col-align-last-right horizontal clearfix tb-scope">
            <div class="stack-width inset-1 inset tb-terminal">
              <div class="inset-2 inset tb-terminal">
                <div class="inset-3 inset tb-terminal">
                  <div class="inset-4 inset tb-terminal">
    
                      <?php 
                        $colCount = 0;
                        $counter = 1;
                        if (!empty($page['preheader_first'])) { $colCount++; }
                        if (!empty($page['preheader_second'])) { $colCount++; }
                        if ($colCount > 1) { $gridClass = ' left'; }
                        if ($colCount == 1) { $gridClass = ' only'; }
                      ?>
                        
                      <div class="box col-<?php echo $colCount; ?> clearfix tb-terminal">
                    
                        <?php if (!empty($page['preheader_first'])) : ?>
                          <div class="col-first col<?php if(isset($gridClass)) { echo $gridClass; } if($colCount == $counter){ echo ' last'; } $counter++; ?> tb-height-balance tb-terminal">
                            <div id="preheader-first-region" class="tb-region tb-scope">
                              <?php print render($page['preheader_first']); ?>
                            </div>
                          </div> <!-- /#preheader-first-region -->
                        <?php endif; ?>
                          
                        <?php if (!empty($page['preheader_second'])) : ?>
                          <div class="col-second col<?php if(isset($gridClass)) { echo $gridClass; } if($colCount == $counter){ echo ' last'; } $counter++; ?> tb-height-balance tb-terminal">
                            <div id="preheader-second-region" class="tb-region tb-scope">
                              <?php print render($page['preheader_second']); ?>
                            </div>
                          </div> <!-- /#preheader-second-region -->
                        <?php endif; ?>
                      
                      <?php unset($colCount); unset($counter); if(isset($gridClass)) { unset($gridClass); } ?>
                      
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        <?php endif; ?>
        
        <?php if ($logo || $site_name || $site_slogan || !empty($page['header'])) : ?>
          <div id="header-inner" class="stack-header-inner stack clearfix tb-scope">
            <div class="stack-width inset-1 inset tb-terminal">
              <div class="inset-2 inset tb-terminal">
                <div class="inset-3 inset tb-terminal">
                  <div class="inset-4 inset tb-terminal">
                    <div class="box col-4 clearfix tb-terminal">
       
                      <?php if (!empty($page['header'])) : ?>
                        <div class=" <?php if (isset($site_name) || isset($site_slogan)) { print 'col col-first right'; } else { print 'col-c tb-primary'; } ?> tb-height-balance tb-terminal">
                          <div id="header-region" class="tb-region tb-scope">
                            <?php print render($page['header']); ?>
                          </div>
                        </div>
                      <?php endif; ?>

                      <?php if (isset($site_name) || isset($logo) || isset($site_slogan)): ?>
                        <div class="col-c tb-primary tb-height-balance">

                          <?php if (isset($logo)): ?>
                            <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" id="logo" class="logo">
                              <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" />
                            </a>
                          <?php endif; ?>

                          <?php if ($site_name): ?>
                            <?php print $wrapper_site_name_prefix; ?>
                              <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home">
                                <span><?php print $site_name; ?></span>
                              </a>
                            <?php print $wrapper_site_name_suffix; ?>
                          <?php endif; ?>

                          <?php if (isset($site_slogan)): ?>
                            <p id="site-slogan" class="site-slogan<?php print ($is_front ? ' site-slogan-front' : ''); ?>"><?php print $site_slogan; ?></p>
                          <?php endif; ?>

                        </div>
                      <?php endif; ?>
                      
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        <?php endif; ?>
        
        <!-- Navigation --> 
        <?php if ($page['navigation']): ?> 
          <div id="navigation" class="stack-navigation stack col-align-right horizontal pulldown clearfix tb-scope">
            <div class="stack-width inset-1 inset tb-terminal">
              <div class="inset-2 inset tb-terminal">
                <div class="inset-3 inset tb-terminal">
                  <div class="inset-4 inset tb-terminal">
                    <div class="box col-1 clearfix tb-terminal">
                      <div id="navigation-region" class="col tb-region tb-scope tb-height-balance">
                        <?php print render($page['navigation']); ?>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        <?php endif; ?>
        
      </div> <!-- /#header -->
      
      <div id="content" class="wrapper-content wrapper clearfix tb-scope">
        <div class="wrapper-1 tb-terminal tb-content-wrapper-1">
          <div class="wrapper-2 tb-terminal tb-content-wrapper-2">
      
            <div id="content-deco-top" class="deco-content-top deco-content deco-top deco tb-scope tb-scope-prefer">
              <div class="width deco-width inner tb-terminal">
                <div class="layer-a layer"></div>
                <div class="layer-b layer"></div>
                <div class="layer-c layer"></div>
                <div class="layer-d layer"></div>
              </div>
            </div>
            
            <div id="content-deco-bottom" class="deco-content-bottom deco-content deco-bottom deco tb-scope tb-scope-prefer">
              <div class="width deco-width inner tb-terminal">
                <div class="layer-a layer"></div>
                <div class="layer-b layer"></div>
                <div class="layer-c layer"></div>
                <div class="layer-d layer"></div>
              </div>
            </div>
          
            <?php if (!empty($page['banner'])) : ?>
              <div id="banner" class="stack-banner stack clearfix tb-scope">
                <div class="stack-width inset-1 inset tb-terminal">
                  <div class="inset-2 inset tb-terminal">
                    <div class="inset-3 inset tb-terminal">
                      <div class="inset-4 inset tb-terminal">
                        <div class="box clearfix tb-terminal">
                          <div id="banner-region" class="col tb-region tb-scope tb-height-balance">
                            <?php print render($page['banner']); ?>
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            <?php endif; ?>
            
            <?php if ($breadcrumb): ?>
              <div id="breadcrumb" class="stack-breadcrumb stack clearfix tb-scope">
                <div class="stack-width inset-1 inset tb-terminal">
                  <div class="inset-2 inset tb-terminal">
                    <div class="inset-3 inset tb-terminal">
                      <div class="inset-4 inset tb-terminal">
                        <div class="box col-1 clearfix tb-terminal">
                          <div id="breadcrumb-region" class="col tb-region tb-scope tb-height-balance">
                            <?php print $breadcrumb; ?>
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            <?php endif; ?>
                
            <?php if ($messages): ?>
              <div id="messages" class="stack-messages stack clearfix tb-scope">
                <div class="stack-width inset-1 inset tb-terminal">
                  <div class="inset-2 inset tb-terminal">
                    <div class="inset-3 inset tb-terminal">
                      <div class="inset-4 inset tb-terminal">
                        <div class="box col-1 clearfix tb-terminal">
                          <div id="messages-region" class="col tb-region tb-scope tb-height-balance">
                            <?php print $messages; ?>
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            <?php endif; ?>
          
            <?php if (!empty($page['precontent_first']) || !empty($page['precontent_second']) || !empty($page['precontent_third'])): ?>
              <div id="precontent" class="stack-precontent stack-pre stack clearfix tb-scope">
                <div class="stack-width inset-1 inset tb-terminal tb-precontent-1">
                  <div class="inset-2 inset tb-terminal tb-precontent-2">
                    <div class="inset-3 inset tb-terminal">
                      <div class="inset-4 inset tb-terminal">
                        
                        <?php
                          $colCount = 0;
                          $counter = 1;
                          if (!empty($page['precontent_first'])) { $colCount++; }
                          if (!empty($page['precontent_second'])) { $colCount++; }
                          if (!empty($page['precontent_third'])) { $colCount++; }
                          if ($colCount > 1) { $gridClass = ' left'; }
                          if ($colCount == 1) { $gridClass = ' only'; }
                        ?>
                          
                        <div class="box col-<?php echo $colCount; ?> clearfix tb-terminal">
                        
                          <?php if(!empty($page['precontent_first'])): ?>
                            <div class="col-first col<?php if(isset($gridClass)) { echo $gridClass; } if($colCount == $counter){ echo ' last'; } $counter++; ?> tb-height-balance tb-terminal">
                              <div id="precontent-first-region" class="tb-region tb-scope">
                                <?php print render($page['precontent_first']); ?>
                              </div>
                            </div>
                          <?php endif; ?>
                          
                          <?php if(!empty($page['precontent_second'])): ?>
                            <div class="col-second col<?php if(isset($gridClass)) { echo $gridClass; } if($colCount == $counter){ echo ' last'; } $counter++; ?> tb-height-balance tb-terminal">
                              <div id="precontent-second-region" class="tb-region tb-scope">
                                <?php print render($page['precontent_second']); ?>
                              </div>
                            </div>
                          <?php endif; ?>
                          
                          <?php if(!empty($page['precontent_third'])): ?>
                            <div class="col-third col<?php if(isset($gridClass)) { echo $gridClass; } if($colCount == $counter){ echo ' last'; } $counter++; ?> tb-height-balance tb-terminal">
                              <div id="precontent-third-region" class="tb-region tb-scope">
                                <?php print render($page['precontent_third']); ?>
                              </div>
                            </div>
                          <?php endif; ?>
                          
                          <?php unset($colCount); unset($counter); if(isset($gridClass)) { unset($gridClass); } ?>
                          
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            <?php endif; ?>

          <?php if (!empty($page['precontent_bottom'])): ?>
            <div id="content-top" class="stack-content-top stack-pre stack clearfix tb-scope">
              <div class="stack-width inset-1 inset tb-terminal">
                <div class="inset-2 inset tb-terminal">
                  <div class="inset-3 inset tb-terminal">
                    <div class="inset-4 inset tb-terminal">
                      <div class="box clearfix tb-terminal">

                      <?php print render($page['precontent_bottom']); ?>

                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          <?php endif; ?>
   
            <div id="content-inner" class="stack-content-inner stack clearfix tb-scope">
              <div class="stack-width inset-1 inset tb-terminal">
                <div class="inset-2 inset tb-terminal">
                  <div class="inset-3 inset tb-terminal">
                    <div class="inset-4 inset tb-terminal">
                      <div class="box clearfix tb-terminal tb-preview-shuffle-regions">
                        
                        <?php 
                          if(isset($variables) && isset($variables['sidebars'])):
                          
                            if(isset($variables['sidebars']['left'])):
                              foreach($variables['sidebars']['left'] as $value):
                                if (!empty($page['sidebar_'.$value])): ?>
                                <div id="sidebar-<?php echo $value; ?>" class="col-<?php echo $value; ?> sidebar left tb-height-balance tb-region tb-scope tb-sidebar tb-left">
                                  <?php print render($page['sidebar_'.$value]); ?>
                                </div>
                            <?php endif; endforeach; endif; ?>
                        
                          <?php 
                            if(isset($variables['sidebars']['right'])):
                              foreach($variables['sidebars']['right'] as $value):
                                if (!empty($page['sidebar_'.$value])): ?>
                                <div id="sidebar-<?php echo $value; ?>" class="col-<?php echo $value; ?> sidebar right tb-height-balance tb-region tb-scope tb-sidebar tb-right">
                                  <?php print render($page['sidebar_'.$value]); ?>
                                </div>
                                
                            <?php endif; endforeach; endif; ?>
                                
                          <?php 
                            if(isset($variables['sidebars']['hidden'])):
                              foreach($variables['sidebars']['hidden'] as $value):
                                if (!empty($page['sidebar_'.$value])): ?>
                                <div id="sidebar-<?php echo $value; ?>" class="col-<?php echo $value; ?> sidebar right tb-height-balance tb-region tb-scope tb-sidebar tb-right tb-hidden">
                                  <?php print render($page['sidebar_'.$value]); ?>
                                </div>
                        
                        <?php endif; endforeach; endif; endif; ?> <!-- end sidebars -->
                        
                        <div id="main" class="col-c tb-height-balance tb-region tb-scope tb-primary">
                          <?php if ($page['highlight']): ?>
                            <div id="highlight">
                              <?php print render($page['highlight']); ?>
                            </div>
                          <?php endif; ?>
                          <div class="pane">
                            <a id="skip-to-content-target" class="skip-to-link" title="Target of skip-to-content link"></a>
                            <?php print render($title_prefix); ?>
                            <?php if ($title): ?>
                              <h1 class="title" id="page-title"><?php print $title; ?></h1>
                            <?php endif; ?>
                            <?php print render($title_suffix); ?>
                            <?php if ($tabs): ?>
                              <div class="tabs"><?php print render($tabs); ?></div>
                            <?php endif; ?>
                            <?php if(!empty($page['help'])) : ?>
                              <div class="help"><?php print render($page['help']); ?></div>
                            <?php endif; ?>
                            <?php if ($action_links): ?>
                              <ul class="action-links"><?php print render($action_links); ?></ul>
                            <?php endif; ?>
                            <?php if(!empty($page['content'])) : ?>
                              <div id="content-area" class="content-area"><?php print render($page['content']); ?></div>
                            <?php endif; ?>
                          </div>
                        </div>
                        
                      </div> <!-- /.box tb-terminal -->
                    </div>
                  </div>
                </div>
              </div>
              
            </div> <!-- /#main -->
            
          </div>
        </div>
      </div> <!-- /#content -->
        
      <?php if (!empty($page['prefooter_first']) || !empty($page['prefooter_second']) || !empty($page['prefooter_third']) || !empty($page['footer_first']) || !empty($page['footer_second'])): ?>
        <div id="footer" class="wrapper-footer wrapper clearfix tb-scope">
        
          <div id="footer-deco-top" class="deco-footer-top deco-footer deco-top deco tb-scope tb-scope-prefer">
            <div class="width deco-width inner tb-terminal">
              <div class="layer-a layer"></div>
              <div class="layer-b layer"></div>
              <div class="layer-c layer"></div>
              <div class="layer-d layer"></div>
            </div>
          </div>
          
          <div id="footer-deco-bottom" class="deco-footer-bottom deco-footer deco-bottom deco tb-scope tb-scope-prefer">
            <div class="width deco-width inner tb-terminal">
              <div class="layer-a layer"></div>
              <div class="layer-b layer"></div>
              <div class="layer-c layer"></div>
              <div class="layer-d layer"></div>
            </div>
          </div>
        
          <?php if (!empty($page['prefooter_first']) || !empty($page['prefooter_second']) || !empty($page['prefooter_third'])): ?>
            <div id="prefooter" class="stack-prefooter stack-pre stack clearfix tb-scope">
              <div class="stack-width inset-1 inset tb-terminal tb-prefooter-1">
                <div class="inset-2 inset tb-terminal tb-prefooter-2">
                  <div class="inset-3 inset tb-terminal">
                    <div class="inset-4 inset tb-terminal">
                    
                      <?php
                        $colCount = 0;
                        $counter = 1;
                        if (!empty($page['prefooter_first'])) { $colCount++; }
                        if (!empty($page['prefooter_second'])) { $colCount++; }
                        if (!empty($page['prefooter_third'])) { $colCount++; }
                        if ($colCount > 1) { $gridClass = ' left'; }
                        if ($colCount == 1) { $gridClass = ' only'; }
                      ?>
                        
                      <div class="box col-<?php echo $colCount; ?> clearfix tb-terminal">
                      
                        <?php if(!empty($page['prefooter_first'])): ?>
                          <div class="col-first col<?php if(isset($gridClass)) { echo $gridClass; } if($colCount == $counter){ echo ' last'; } $counter++; ?> tb-height-balance tb-terminal">
                            <div id="prefooter-first-region" class="tb-region tb-scope">
                              <?php print render($page['prefooter_first']); ?>
                            </div>
                          </div>
                        <?php endif; ?>
                        
                        <?php if(!empty($page['prefooter_second'])): ?> 
                          <div class="col-second col<?php if(isset($gridClass)) { echo $gridClass; } if($colCount == $counter){ echo ' last'; } $counter++; ?> tb-height-balance tb-terminal">
                            <div id="prefooter-second-region" class="tb-region tb-scope">
                              <?php print render($page['prefooter_second']); ?>
                            </div>
                          </div>
                        <?php endif; ?>
                        
                        <?php if(!empty($page['prefooter_third'])): ?>
                          <div class="col-third col<?php if(isset($gridClass)) { echo $gridClass; } if($colCount == $counter){ echo ' last'; } $counter++; ?> tb-height-balance tb-terminal">
                            <div id="prefooter-third-region" class="tb-region tb-scope">
                              <?php print render($page['prefooter_third']); ?>
                            </div>
                          </div>
                        <?php endif; ?>
                        
                        <?php unset($colCount); unset($counter); if(isset($gridClass)) { unset($gridClass); } ?>
                        
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          <?php endif; ?>
      
          <?php if (!empty($page['footer_first']) || !empty($page['footer_second'])): ?>
            <div id="footer-inner" class="stack-footer-inner stack clearfix tb-scope">
              <div class="stack-width inset-1 inset tb-terminal">
                <div class="inset-2 inset tb-terminal">
                  <div class="inset-3 inset tb-terminal">
                    <div class="inset-4 inset tb-terminal">
                      
                      <?php
                        $colCount = 0;
                        $counter = 1;
                        if (!empty($page['footer_first'])) { $colCount++; }
                        if (!empty($page['footer_second'])) { $colCount++; }
                        if ($colCount > 1) { $gridClass = ' left'; }
                        if ($colCount == 1) { $gridClass = ' only'; }
                      ?>
                        
                      <div class="box col-<?php echo $colCount; ?> clearfix tb-terminal">
                      
                        <?php if (!empty($page['footer_first'])) : ?>
                          <div class="col-first col<?php if(isset($gridClass)) { echo $gridClass; } if($colCount == $counter){ echo ' last'; } $counter++; ?> tb-height-balance tb-terminal">
                            <div id="footer-first-region" class="tb-region tb-scope">                          
                              <?php print render($page['footer_first']); ?>
                            </div>
                          </div>
                        <?php endif; ?>
                          
                        <?php if (!empty($page['footer_second'])) : ?>
                          <div class="col-second col<?php if(isset($gridClass)) { echo $gridClass; } if($colCount == $counter){ echo ' last'; } $counter++; ?> tb-height-balance tb-terminal">
                            <div id="footer-second-region" class="tb-region tb-scope">
                              <?php print render($page['footer_second']); ?>
                            </div>
                          </div>
                        <?php endif; ?>
                        
                        <?php unset($colCount); unset($counter); if(isset($gridClass)) { unset($gridClass); } ?>
                      
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          <?php endif; ?>
              
        </div> <!-- /#footer -->
      <?php endif; ?>
      
    </div> <!-- /.lining -->
  </div> <!-- /.page-width -->
</div> <!-- /#page -->
    
<?php if (!empty($page['copyright'])) : ?> 
  <div id="copyright" class="stack-copyright stack page clearfix tb-region tb-scope">
    <div class="page-width inner">
      <div class="stack-width">
        <div class="box">
          <?php print render($page['copyright']); ?>
        </div>
      </div>
    </div>
  </div>
<?php endif; ?>
