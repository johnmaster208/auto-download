<?php
/*
* autodownload_configure_pages()
* returns listing of all node/page download assignments with 'enabled' toggler checkbox
*/

/*
*list_current_pages
*shows existing assignments for auto-downloads
*/
function autodownload_list_current_pages(){
    //create form elements
    
    $headers = array(
        array('data' => t('Page')),  
        array('data' => t('Links To...')),  
        array('data' => t('Enabled')),
        array('data' => t('Edit')),
        array('data' => t('Delete')),  
      );
    
    $caption =   'Listed below are the nodes which currently have automatic downloads assigned to them. 
      For this module version, only one (1) download per node may be assigned. If you create a new auto-download assignment 
      on a node which already has one, the pre-existing assignment will be lost. To edit an existing automatic 
      download configuration, click the "edit" link to the right of the node.';
    
    //query the db for data
    $result = db_query("SELECT * FROM {autodownloads}");
    
    foreach($result as $row){
    //grab the page
    $form['list_current']['node'] = array(
      '#markup' => $row->nid,
      '#default_value' => $row->nid,
    );
    //grab the url
    $form['list_current']['url'] = array(
      '#markup' => $row->url,
      '#default_value' => $row->url,
    ); 
    //grab the default checkbox value for enabled
    $form['list_current']['enabled'] = array(
      '#title' => t('Enabled'),
      '#type' => 'checkbox',
      '#default_value' => $row->enabled, 
    );
    
    $rows[] = array(
        array('data' => $form['list_current']['node']),
        array('data' => $form['list_current']['url']),
        array('data' => $form['list_current']['enabled']),
        array('data' => l(t('Edit'), 'admin/config/user-interface/autodownload/edit' . '/' . $row->dlid)),
        array('data' => l(t('Delete'), 'admin/config/user-interface/autodownload/delete' . '/' . $row->dlid)),
      );
    }//end foreach
     
return theme('table', array('header' => $headers,'rows' => $rows, 'caption' => $caption));
}

/*
*create_new_download
*allows admins to add a new assignment
*/
function autodownload_create_new_download_form($form, &$form_state){
    $form['new_download']['settings'] = array(
        '#type' => 'fieldset',
        '#title' => 'Create New Auto-download assignment',
        '#description' => 'Fill out the fields to create a new auto-download',
        '#collapsible' => TRUE,       
        );
    $form['new_download']['settings']['page'] = array(
        '#type' => 'textfield',
        '#name' => 'page', 
        '#title' => t('Page or Node ID'),
        '#description' => t('Enter the name of the page or node in this field. It can be a node ID, such as node/123 or an alias, like products/phone-systems.'),
        '#length' => 150,
        '#element_validate' => array('autodownload_create_new_download_form_validate'),
        );
    $form['new_download']['settings']['url'] = array(
        '#type' => 'textarea',
        '#name' => 'url',
        '#title' => t('File to be downloaded'),
        '#description' => t('Enter a link to the file which will be automatically downloaded.'),
        '#cols' => 50,
        '#rows' => 5,
        '#resizable' => TRUE,
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Create Auto-download'),
    );
    return $form;
}

function autodownload_create_new_download_form_validate($element, &$form_state){
    //add this later
}
function autodownload_create_new_download_form_submit($form, &$form_state){
    //add the new entry as a record to {autodownloads}
    $table = 'autodownloads';
    $record = new stdClass();
    $record->nid = $form_state['values']['page'];
    $record->url = $form_state['values']['url'];
    $success = drupal_write_record($table, $record);
     
     if($success){
         $form_state['redirect'] = 'admin/config/user-interface/autodownload/list';
         drupal_set_message('The automatic download assignment was created.');  
     }
    else{
        drupal_set_message("There was a problem adding an assignment. Please contact the admin for details.", "error");
    }
}

/*
* autodownload_edit_node_form()
* 
*/
function autodownload_edit_node_form($form, &$form_state, $dlid){
    //query the {autodownloads} table for info related to our parameter
    $result = db_query("SELECT * FROM {autodownloads} WHERE {autodownloads}.dlid = :dlid", array(':dlid' => $dlid));
    $form['edit_download']['settings'] = array(
         '#type' => 'fieldset',
         '#title' => 'Edit this Auto-download assignment',
         '#description' => 'Fill out the fields to create a new auto-download',
         '#collapsible' => FALSE,       
         );
    foreach($result as $row){
     $form['edit_download']['settings']['page'] = array(
         '#type' => 'textfield',
         '#name' => 'page', 
         '#title' => t('Page or Node ID'),
         '#description' => t('Enter the name of the page or node in this field. It can be a node ID, such as node/123 or an alias, like products/phone-systems.'),
         '#length' => 150,
         '#default_value' => $row->nid,
         );
     $form['edit_download']['settings']['url'] = array(
         '#type' => 'textarea',
         '#name' => 'url',
         '#title' => t('File to be downloaded'),
         '#description' => t('Enter a link to the file which will be automatically downloaded.'),
         '#cols' => 50,
         '#rows' => 5,
         '#resizable' => TRUE,
         '#default_value' => $row->url,
         );
     }//end foreach
     $form['submit'] = array(
         '#type' => 'submit',
         '#value' => t('Save Configuration'),
         );
     return $form;
}

function autodownload_delete_node_form($form, &$form_state, $dlid){
    //query the {autodownloads} table for info related to our parameter
    $result = db_query("SELECT * FROM {autodownloads} WHERE 'dlid' = $dlid");
    foreach($result as $row){
    $form['delete_download']['settings'] = array(
         '#type' => 'fieldset',
         '#title' => 'Create New Auto-download assignment',
         '#description' => 'Fill out the fields to create a new auto-download',
         '#collapsible' => TRUE,       
         );
     $form['delete_download']['settings']['page'] = array(
         '#type' => 'textfield',
         '#name' => 'page', 
         '#title' => t('Page or Node ID'),
         '#description' => t('Enter the name of the page or node in this field. It can be a node ID, such as node/123 or an alias, like products/phone-systems.'),
         '#length' => 150,
         '#default_value' => $row->nid,
         );
     $form['delete_download']['settings']['url'] = array(
         '#type' => 'textarea',
         '#name' => 'url',
         '#title' => t('File to be downloaded'),
         '#description' => t('Enter a link to the file which will be automatically downloaded.'),
         '#cols' => 50,
         '#rows' => 5,
         '#resizable' => TRUE,
         '#default_value' => $row->url,
         );
     }//end foreach
     $form['submit'] = array(
         '#type' => 'submit',
         '#value' => t('Save Configuration'),
         );
     return $form;
}


/*
* autodownload_configure_advanced_form()
* additional settings for auto-downloads; timeOut, UI Dialogs and enhanced library integration
*/

function autodownload_configure_advanced_form($form, &$form_state){
    
    $form['advanced_settings']['javascript'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#title' => 'JavaScript Settings', 
    );
    $form['advanced_settings']['javascript']['enable_timeouts'] = array(
      '#type' => 'checkbox',
      '#name' => 'autodl_enable_timeouts',
      '#value' => variable_get('autodl_timeouts_enable',0),
      '#title' => 'Enable setTimeout function for node pages.',
      '#description' => 'Enabling this option will allow you to set a timer on all auto-downloads. The setTimeout function will default to 5 seconds (5000ms).',
      '#default' => FALSE, 
    );
    if(variable_get('auto_dl_timeouts_enable') == 1){
    $form['advanced_settings']['javascript']['timeout_timer'] = array(
        '#type' => 'textfield',
        '#name' => 'autodl_timeout_timer',
        '#value' => variable_get('autodl_timeout_timer',5),
        '#title' => 'Timeout for auto-downloads',
        '#description' => 'Enter a number of seconds to wait until the download begins.',
    );
    }

    $form['advanced_settings']['javascript']['dialogs'] = array(
        '#type' => 'checkbox',
        '#name' => 'autodl_enable_dialogs',
        '#value' => variable_get('autodl_dialogs_enable',0),
        '#title' => 'Enable jQuery UI Dialog popups for File Downloads',
        '#description' => 'Enabling this option will '
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save Configuration'),
    );
    return $form;
}
function autodownload_configure_advanced_form_submit($form, &$form_state){
    
    if ($form_state['values']['autodl_enable_timeouts'] == 1){
    variable_set('autodl_timeouts_enable',1);
    drupal_set_message("Autodownload timeouts have been enabled.");
    }
    else{
    variable_set('autodl_timeouts_enable',0);
    drupal_set_message("Autodownload timeouts have been disabled.");
    }
    
}


/* $form['list_current']['current_nodes']['submit'] = array(
     '#type' => 'submit',
     '#value' => 'Save Configuration',  
   );*/

?>