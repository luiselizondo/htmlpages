<?php


/**
 * Shows all entities in a table
 */
function _list_entities() {
  $content = array();
  $entities = htmlpages_load_multiple();

  if(!empty($entities)) {
    foreach ($entities as $key => $entity) {
      $account = user_load($entity->uid);
      $rows[] = array(
        'data' => array(
          'title' => l($entity->title, 'pages/' . $entity->machine_name),
          'author' => l(format_username($account), "user/" . $account->uid),
          'created' => format_date($entity->created),
          'path' => l($entity->path, $entity->path),
          'status' => $entity->status == 0 ? t("Not published") : t("Published"),
          'view' => l(t("View"), $entity->path),
          'edit' => l(t("Edit"), 'admin/structure/htmlpages/' . $entity->id . '/edit'),
          'delete' => l(t("Delete"), 'admin/structure/htmlpages/' . $entity->id . '/delete')
        )
      );
    }

    $content["entity_table"] = array(
      '#theme' => 'table',
      '#rows' => $rows,
      '#header' => array(
        t("Title"),
        t("Author"),
        t("Created"),
        t("Path"),
        t("Status"),
        t("View"),
        t("Edit"),
        t("Delete")
      )
    );
  }
  else {
    $content[] = array(
      '#type' => 'item',
      '#markup' => t("There's no content to load"),
    );
  }

  return $content;
}

/**
 * Callback to show all HTML entities
 */
function htmlpages_admin_page() {
  $content = array();

  $content["table"] = _list_entities();

  return $content;
}