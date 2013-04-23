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
    /*$form['list_current']['current_nodes'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#title' => 'Current Nodes with Autodownloads',
    '#description' => 'Listed below are the nodes which currently have automatic downloads assigned to them. 
    For this module version, only one (1) download per node may be assigned. If you create a new auto-download assignment 
    on a node which already has one, the pre-existing assignment will be lost. To edit an existing automatic 
    download configuration, click the "edit" link to the right of the node.', 
    );*/
    //do query to grab all existing download/node assignments
    $result = db_query("SELECT * FROM {autodownloads}");
    //Add Create New Download+ link at the top right
    $output = "<a style='text-decoration:none; float:right;' href=\"autodownload/create\">Create New+</a>";
    //output results in dynamic table
    $output .= "<h1>Current Automatic Downloads</h1>";
    $output .= "<table>";
    //table headers
    $output .= "<th>Page</th>";
    $output .= "<th>Automatically Downloads this file</th>";
    $output .= "<th><!--Edit Link header--></th>";
    $output .= "<th><!--Delete Link header--></th>";
    foreach($result as $row){
    //table rows
    $output .= "<tr>";
      $output .= "<td>$row->nid</td>";
      $output .= "<td>$row->url</td>";
      $output .= "<td><a href=\"edit?dl=$row->nid\">Edit</a></td>";
      $output .= "<td><a href-'#'>Delete</a></td>";
    $output .= "</tr>";
    }
     $output .= "</table>";
     
     return $output;
}

/*
*create_new_download
*allows admins to add a new assignment
*/
function autodownload_create_new_download_form(){
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
         $form_state['redirect'] = 'admin/config/user-interface/autodownload/settings';
         drupal_set_message('The automatic download assignment was created.');  
     }
    else{
        drupal_set_message("There was a problem adding an assignment. Please contact the admin for details.", "error");
    }
}

/*
*edit_existing_download
* accepts passed $assignment and edits the existing form
*/
function autodownload_edit_existing_download_form($assignment){
    $assignment = $_REQUEST['dl'];
    //query the {autodownloads} table for info related to our parameter
    $result = db_query("SELECT * FROM {autodownloads} WHERE 'nid' = $assignment");
    //build the form
    $form['existing_download']['settings'] = array(
      '#type' => 'fieldset',
      '#title' => "Edit the \'$result->nid\' assignment",
      '#description' => 'Fill out the fields to change this assignment',
      '#collapsible' => TRUE,       
    );
    return $form;
}

/*
* autodownload_edit_node_form()
* 
*/
function autodownload_edit_node_form(){
    
}

/*
* autodownload_configure_advanced_form()
* additional settings for auto-downloads; timeOut, UI Dialogs and enhanced library integration
*/

function autodownload_configure_advanced_form(){
    
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

?>