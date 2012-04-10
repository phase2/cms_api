<?php

/**
 * Main entry point for perlgem callback
 *
 * @type string
 *  The type of the content to get.  Can be block, node or page.
 */
function cms_api_get() {
  drupal_theme_initialize();

  $args = func_get_args();
  $type = array_shift($args);

  if (empty($args)) {
    $function = __FUNCTION__ . "_{$type}_list";
  }
  else {
    $function = __FUNCTION__ . "_{$type}";
  }

  $result = array(
    'status' => FALSE,
    'data' => array(),
    'css' => array(),
    'js' => array(),
  );

  if (function_exists($function)) {
    $result['status'] = TRUE;
    $result['data'] = call_user_func_array($function, $args);
  }

  $css = drupal_get_css();
  $js = drupal_get_js();
  $result['css'] = $css;
  $result['js'] = $js;
  $result['css_array'] = drupal_add_css();
  $result['js_array'] = drupal_add_js();

  drupal_json_output($result);
}

function cms_api_get_block($module, $delta) {
  $blocks = array();
  $block_object = block_load($module, $delta);

  $block_object->enabled = $block_object->page_match = TRUE;
  $blocks[$block_object->module . "_" . $delta] = $block_object;
  $block_content = _block_render_blocks($blocks);
  $build = _block_get_renderable_array($block_content);

  return drupal_render($build);
}

function cms_api_get_block_list() {
  $query = db_select('block', 'b');
  $result = $query
    ->fields('b')
    ->orderBy('b.region')
    ->orderBy('b.weight')
    ->orderBy('b.module')
    ->addTag('block_load')
    ->addTag('translatable')
    ->execute();

  $block_info = $result->fetchAllAssoc('bid');
  // Allow modules to modify the block list.
  drupal_alter('block_list', $block_info);

  return $block_info;
}

function cms_api_get_region() {
  $render = array();
  $args = func_get_args();

  if (count($args) > 1) {
    foreach ($args as $region) {
      $render[$region]['build'] = block_get_blocks_by_region($region);
      $render[$region]['html'] = render(block_get_blocks_by_region($region));
    }
  }
  else {
    $render['html'] = render(block_get_blocks_by_region($args[0]));
    $render['build'] = block_get_blocks_by_region($args[0]);
  }

  return $render;
}

function cms_api_get_region_list() {
  global $theme_key;

  return system_region_list($theme_key);
}

function cms_api_get_section() {
  $return = array();

  $args = func_get_args();
  if (count($args) > 1) {
    foreach ($args as $section) {
      $return[$section] = _cms_api_get_section_section($section);
    }
  }
  else {
    $return =  _cms_api_get_section_section($args[0]);
  }

  return $return;
}

function _cms_api_get_section_section($section) {
  static $sections = array();

  if (empty($sections[$section])) {
    if ($section_cache = cache_get("CMS::API::SECTION::$section")) {
      $sections[$section] = $section_cache->data;
    }
    else {
      // Get the page html
      $request = _cms_api_get_get_section_page();
      if ($request) {
        $path = libraries_get_path('QueryPath');
        require_once $path . '/QueryPath.php';

        $qp = htmlqp($request);

        $sections[$section] = $qp->find($section)->html();
        cache_set("CMS::API::SECTION::$section", $sections[$section]);
      }
      else {
        $sections[$section] = FALSE;
      }
    }
  }

  return $sections[$section];
}

function _cms_api_get_get_section_page($url = '') {
  static $pages = array();

  if (empty($pages[$url])) {
    if ($page_cache = cache_get("CMS::API::PAGE::$url")) {
      $pages[$url] = $page_cache->data;
    }
    else {
      $orig_path = $_GET['q'];
      $_GET['q'] = $url;

      drupal_path_initialize();
      $page_array = menu_execute_active_handler($_GET['q'], FALSE);
      // Send appropriate HTTP-Header for browsers and search engines.
      global $language;
      drupal_add_http_header('Content-Language', $language->language);
      $page_html = drupal_render_page($page_array);
      cache_set("CMS::API::PAGE::$url", $page_html);

      $_GET['q'] = $orig_path;

      $pages[$url] = $page_html;
    }
  }

  return $pages[$url];
}
