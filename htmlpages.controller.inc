<?php

class HTMLPagesController extends DrupalDefaultEntityController {
  public function save($entity) {
    drupal_write_record("htmlpages", $entity);
    field_attach_insert("htmlpage", $entity);
    module_invoke_all("entity_insert", "htmlpage", $entity);
    return $entity;
  }

  public function update($entity) {
   $primary_keys = 'id';
   drupal_write_record("htmlpages", $entity, $primary_keys);
   field_attach_update('htmlpage', $entity);
   module_invoke_all("entity_update", "htmlpage", $entity);
   return $entity;
  }

  public function delete($entity) {
    // Delete path
    if(module_exists("path") && !empty($entity->pid)) {
      path_delete($entity->pid);
    }

    $delete = db_delete("htmlpages")
    ->condition("id", $entity->id, "=")
    ->execute();

    field_attach_delete('htmlpage', $entity);
    module_invoke_all("entity_delete", "htmlpage", $entity);

    watchdog("htmlpages", "Deleted HTML Page with ID: " . $entity->id);

    menu_rebuild();
    return $delete;
  }
}