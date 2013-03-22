<?php


/**
 * Provides a wrapper on the edit form to add a new entity.
 */
function htmlpages_add() {
  global $user;
  $page = (object) array(
    'id' => '',
    'type' => 'htmlpage',
    'uid' => $user->uid,
    'created' => time(),
    'machine_name' => '',
    'content' => '',
    'css' => '',
    'path' => '',
    'pid' => '',
    'title' => '',
    'menu_name' => 'main-menu',
    'access_arguments' => 'view html pages',
    'variables' => '',
    'status' => '',
  );

  return drupal_get_form('htmlpages_add_form', $page);
}

/**
 * Wrapper arround the htmlpages_add_form to edit an existing htmlpage
 */
function htmlpages_edit($page) {
  global $user;
  return drupal_get_form("htmlpages_add_form", $page);
}


/**
 * Wrapper arround htmlpages_delete_form()
 */
function htmlpages_delete($page) {
  global $user;
  return drupal_get_form("htmlpages_delete_form", $page);
}

/**
 * Form callback to confirm deletion
 */
function htmlpages_delete_form($form, &$form_state, $entity) {
  return confirm_form(array(
    'htmlpage' => array(
      '#type' => 'value',
      '#value' => $entity->id,
    ),
  ), t('Are you sure you want to delete the HTML page %title?', array('%title' => $entity->title)), 'pages/' . $entity->machine_name, t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

function htmlpages_delete_form_submit($form, &$form_state) {
  $values = (object) $form_state["values"];
  $id = $values->htmlpage;
  $entity = htmlpages_load($id);
  
  if(isset($entity)) {
    htmlpages_remove($entity);
    drupal_set_message(t("Deleted HTML page !title", array("!title" => $entity->title)));
    $form_state["redirect"] = "/admin/structure/htmlpages";
  }
}

/**
 * Build a list of permissions
 */
function _getPermissions() {
  $module_info = system_get_info('module');
  $modules = array();
  foreach (module_implements('permission') as $module) {
    $modules[$module] = $module_info[$module]['name'];
  }
  asort($modules);

  $permissions = module_invoke_all('permission');

  foreach ($permissions as $key => $perm) {
    $items[$key] = $perm["title"];
  }

  return $items;
}

/**
 * Check if a page already exists
 */
function htmlpages_item_exists($value) {
  $exists = db_query_range("SELECT 1 FROM {htmlpages} WHERE machine_name = :machine_name", 0, 1, array(":machine_name" => $value))->fetchField();
  return $exists;
}

/**
 * Form callback to add new pages
 */
function htmlpages_add_form($form, &$form_state, $entity) {
  // add javascript to handle tabs
  drupal_add_js(drupal_get_path("module", "htmlpages") . "/tabs.js");

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#required' => TRUE,
    '#default_value' => isset($entity->title) ? $entity->title : NULL,
  );

  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Machine name'),
    '#machine_name' => array(
      'exists' => 'htmlpages_item_exists',
      'source' => array("title"),
      'label' => 'Machine name',
      'replace_pattern' => '[^a-z0-9_]+',
      'replace' => '_'
    ),
    '#required' => TRUE,
    '#default_value' => isset($entity->machine_name) ? $entity->machine_name : NULL,
    '#disabled' => !empty($entity->machine_name) ? TRUE : FALSE,
  );

  $form['content'] = array(
    '#type' => 'textarea',
    '#title' => t('HTML Content'),
    '#required' => TRUE,
    '#rows' => 30,
    '#default_value' => isset($entity->content) ? $entity->content : NULL,
  );

  $form['css'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS'),
    '#required' => FALSE,
    '#rows' => 30,
    '#default_value' => isset($entity->css) ? $entity->css : NULL,
  );
  

  $form["group_htmlpages"] = array(
    '#type' => 'vertical_tabs',
  );

  $form["menu"] = array(
    '#type' => 'fieldset',
    '#title' => t('Menu'),
    '#group' => "group_htmlpages"
  );

  $form["menu"]["menu_name"] = array(
    '#type' => 'select',
    '#options' => menu_get_menus(),
    '#title' => t("Menu parent"),
    '#default_value' => isset($entity->menu_name) ? $entity->menu_name : NULL,
  );

  if(module_exists("path")) {
    $form["menu"]['path'] = array(
      '#type' => 'textfield',
      '#title' => t('URL Path'),
      '#required' => TRUE,
      '#default_value' => isset($entity->path) ? $entity->path : NULL,
    );
  }

  $form["permissions"] = array(
    '#type' => 'fieldset',
    '#title' => t('Permissions'),
    '#group' => "group_htmlpages"
  );  

  $form["permissions"]["access_arguments"] = array(
    '#type' => 'select',
    '#title' => t('Permissions'),
    '#options' => _getPermissions(),
    '#default_value' => isset($entity->access_arguments) ? $entity->access_arguments : NULL,
  );

  $form["advanced"] = array(
    '#type' => 'fieldset',
    '#title' => t("Advanced"),
    '#group' => 'group_htmlpages'
  );

  $form["advanced"]['variables'] = array(
    '#type' => 'textarea',
    '#title' => t('Variables'),
    '#required' => FALSE,
    '#default_value' => isset($entity->variables) ? $entity->variables : NULL,
  );

  $form['basic_entity'] = array(
    '#type' => 'value',
    '#value' => $entity,
  );

  $form["actions"] = array(
    '#prefix' => '<div class="form-actions">',
    '#suffix' => '</div>',
    '#weight' => 100,
  );

  $form["actions"]['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save and publish'),
    '#weight' => 100,
  );

  $form["actions"]['draft'] = array(
    '#type' => 'submit',
    '#value' => t('Save as draft and preview'),
    '#weight' => 101,
  );

  if(!empty($entity->id)) {
    $form["_edit"] = array("#type" => "hidden", "#value" => $entity->id);
    if(isset($entity->pid)) {
      $form["pid"] = array("#type" => 'hidden', "#value" => $entity->pid);
    }
    $form["status"] = array(
      '#prefix' => '<div class="actions">',
      '#suffix' => '</div>',
      '#markup' => t("Current status is: !status", array("!status" => $entity->status == 0 ? t("Not published") : t("Published"))),
      '#weight' => 99,
    );
    $form["actions"]["submit"]["#value"] = t("Update and publish");
    $form["actions"]["draft"]["#value"] = t("Update and preview");
    $form["actions"]['delete'] = array(
      '#markup' => l(t("Delete"), "admin/structure/htmlpages/" . $entity->id . "/delete", array("attributes" => array("class" => "btn btn-danger"))),
      '#weight' => 110
    );
  }
 
  field_attach_form('htmlpage', $entity, $form, $form_state);

  return $form;
}

function _saveAlias($entity) {
  $path = array();
  $path['alias'] = trim($entity->path);
  // Only save a non-empty alias.
  if (!empty($path['alias'])) {
    // Ensure fields for programmatic executions.
    $langcode = entity_language('htmlpage', $entity);
    $path['source'] = 'pages/' . $entity->machine_name;
    $path['language'] = isset($langcode) ? $langcode : LANGUAGE_NONE;
    path_save($path);

    return path_load($path['source']);
  }
}

/**
 * SUbmit callback for htmlpages_add_form()
 */
function htmlpages_add_form_submit($form, &$form_state) {
  global $user;
  $values = (object) $form_state["values"];

  $entity = new stdClass();
  $entity->machine_name = $values->machine_name;
  $entity->title = $values->title;
  $entity->created = time();
  $entity->variables = $values->variables;
  $entity->content = $values->content;
  $entity->css = $values->css;
  $entity->access_arguments = $values->access_arguments;
  $entity->menu_name = $values->menu_name;
  
  if($values->op == "Save as draft and preview" || $values->op == "Update and preview") {
    $entity->status = 0;
  }
  else {
    $entity->status = 1;
  }

  // update
  if(!empty($values->_edit)) { 
    $entity->id = $values->_edit;
    $entity->pid = !empty($values->pid) ? $values->pid : NULL;

    if(module_exists("path")) {
      $alias = trim($values->path);
      $pid = $values->pid;
      // Delete old alias if user erased it.
      if (!empty($pid) && empty($alias)) {
        path_delete($pid);
      }

      drupal_set_message("Saving path");
      $path = _saveAlias($entity);
      $entity->pid = $path["pid"];
    }

    field_attach_submit("htmlpage", $entity, $form, $form_state);
    $htmlpage = htmlpages_update($entity);
  }
  else {
    if(module_exists("path")) {
      $entity->path = $values->path;
      $path = _saveAlias($entity);
      $entity->pid = $path["pid"];
    }

    $entity->uid = $user->uid;
    $entity->created = time();
    field_attach_submit("htmlpage", $entity, $form, $form_state);

    $htmlpage = htmlpages_save($entity);
  }

  menu_rebuild();
  // Check the status and redirect to the page if this is a preview or redirect to the admin list page if it is published
  if($entity->status == 0) {
    $form_state["redirect"] = "pages/" . $entity->machine_name;
  }
  else {
    drupal_set_message(t("Saved and published the HTML page !title", array("!title" => l($entity->title, "pages/" . $entity->machine_name))));
    $form_state["redirect"] = "admin/structure/htmlpages";
  }
}