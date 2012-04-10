The CMS API module provides a very simple JSON API into drupal.  This was used to pull in parts of drupal into another site.

Return JSON format:

{
  'status': TRUE/FALSE
  'data': {} Data from endpoint
  'css': '' CSS that is included in the html page @see drupal_get_css()
  'js': ''  JS that is included in the html page @see drupal_get_js()
  'css_array': {} The CSS render array.  @see drupal_add_css()
  'js_array': {} The JS render array. @see drupal_add_js()
}

Current endpoints:

cms-api/block
- A list of blocks

cms-api/block/<module>/<delta>
- The Block html

cms-api/region
- A list of regions

cms-api/region/<region name>
- The HTML for the region

cms-api/section/<section_name>
- The HTML for the <div> from the sites home page
- For example, getting the header region since lots of this is hard coded in page.tpl.php
